/**
 * parallel_make
 * CS 341 - Spring 2024
 */

// Code reused from Fall 2023
// Accidentally submitted unfinished code for part 1. The one time I didn't double-check my old code.
// Lmao.

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "queue.h"
#include "set.h"

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>

#include <pthread.h>

static queue *waiting_rules;
static size_t queue_size;

static set *all_rules;
static set *fresh_rules;

static set *failed_rules;
static set *successful_rules;

static pthread_mutex_t verify_mtx;
static pthread_cond_t verify_cond;
static pthread_mutex_t queue_mtx;
static pthread_cond_t queue_cond;

static bool job_done;

void print_vector(vector *vect) {
    size_t i;
    for (i = 0; i < vector_size(vect); i++) {
        printf("%s ", (char *)vector_get(vect, i));
    }
    printf("\n");
}

bool vector_contains_string(vector *vect, char *string) {
    size_t i;
    for (i = 0; i < vector_size(vect); i++) {
        if (strcmp(vector_get(vect, i), string) == 0) {
            return true;
        }
    }
    return false;
}

bool DFS_verify(graph *dependency_graph, vector *black, vector *grey, char *cur_node) {
    // If the node is black, then it doesn't contain a cycle
    if (vector_contains_string(black, cur_node)) {
        return false;
    }
    // If the node is grey, then it does contain a cycle
    else if (vector_contains_string(grey, cur_node)) {
        return true;
    }
    vector_push_back(grey, cur_node);
    vector *cur_vect = graph_neighbors(dependency_graph, cur_node);
    size_t i;
    for (i = 0; i < vector_size(cur_vect); i++) {
        char *child_node = vector_get(cur_vect, i);         // These should be shallow copies
        if (DFS_verify(dependency_graph, black, grey, child_node)) {
            return true;
        }
    }
    vector_push_back(black, cur_node);

    vector_pop_back(grey);

    free(vector_begin(cur_vect));
    free(cur_vect);
    cur_vect = NULL;
    return false;
}

bool contains_cycle(graph *dependency_graph, char *root) {
    vector *black = string_vector_create();
    vector *grey = string_vector_create();

    bool result = DFS_verify(dependency_graph, black, grey, root);

    vector_destroy(black);
    black = NULL;

    vector_destroy(grey);
    grey = NULL;

    return result;
}

// Threadless
bool DFS_run(graph *dependency_graph, vector *visited, char *cur_node) {
    // If the node has been visited, then it doesn't need to be run again
    if (vector_contains_string(visited, cur_node)) {
        return true;
    }
    vector_push_back(visited, cur_node);
    
    // Check if the cur_node is the name of a file on the disk
    struct stat cur_data;
    bool is_file = stat(cur_node, &cur_data) == 0;
    bool children_are_files = true;
    bool child_is_older = false;
    vector *cur_vect = graph_neighbors(dependency_graph, cur_node);
    size_t i;
    if (is_file) {
        // Check if all of the cur_node's dependencies are the names of files on the disk
        for (i = 0; i < vector_size(cur_vect) && children_are_files && !child_is_older; i++) {
            struct stat child_data;
            children_are_files = children_are_files && (stat(vector_get(cur_vect, i), &child_data) == 0);
            // Check if a child file has a newer modification time than the current file
            if (children_are_files && (difftime(cur_data.st_mtime, child_data.st_mtime) < 0)) {
                child_is_older = true;
            }
        }
    }

    bool result = true;
    // Try to run all of its children
    for (i = 0; i < vector_size(cur_vect); i++) {
        char *child_node = vector_get(cur_vect, i);         // These should be shallow copies
        if (!DFS_run(dependency_graph, visited, child_node)) {
            result = false;
        }
    }
    
    // Once it's successful, free cur_vect and its pointer array of neighbors
    free(vector_begin(cur_vect));
    free(cur_vect);
    cur_vect = NULL;

    // If cur_data is not a file, its children are not files, or a child file is older, then run its commands
    if ((!is_file || !children_are_files || child_is_older) && result) {
        rule_t *rule = (rule_t *)graph_get_vertex_value(dependency_graph, cur_node);
        vector *commands = rule->commands;
        for (i = 0; i < vector_size(commands); i++) {
            if(system(vector_get(commands, i)) != 0) {
                result = false;
                break;
            }
        }
        rule = NULL;
        commands = NULL;
    }
    return result;
}

bool run_target_threadless(graph *dependency_graph, char *root) {
    vector *visited = string_vector_create();

    bool result = DFS_run(dependency_graph, visited, root);

    vector_destroy(visited);
    visited = NULL;
    
    return result;
}

// First layer should have "d c e1 e2"
bool find_children(graph *dependency_graph, char *cur_node) {
    vector *dependencies = graph_neighbors(dependency_graph, cur_node);
    size_t i;
    // Add all of cur_node's dependencies to the end of the vector
    bool children_full = true;
    for (i = 0; i < vector_size(dependencies); i++) {
        char *child_node = vector_get(dependencies, i);         // These should be shallow copies
        if (!set_contains(all_rules, child_node)) {
            set_add(fresh_rules, child_node);
            children_full = false;
            find_children(dependency_graph, child_node);
        }
        if (set_contains(fresh_rules, child_node)) {
            children_full = false;
        }
    }
    if (children_full) {
        set_add(all_rules, cur_node);
        queue_size++;
        queue_push(waiting_rules, cur_node);
    }

    // Once it's successful, free cur_vect and its pointer array of neighbors
    free(vector_begin(dependencies));
    free(dependencies);
    dependencies = NULL;
    return children_full;
}

// cur_node is not freed by the function. All strings added are shallow copies
void create_waitlist(graph *dependency_graph, char *root) {
    bool root_added = false;
    do {
        fresh_rules = string_set_create();
        root_added = find_children(dependency_graph, root);
        set_destroy(fresh_rules);
        fresh_rules = NULL;
    } while (!root_added);
}

// root is used as a shallow copy
void run_target(graph *dependency_graph, char *root) {
    // Initialize the values

    create_waitlist(dependency_graph, root);
    pthread_cond_broadcast(&queue_cond);
    
    // Check if the root has been verified, and wait if it hasn't.
    pthread_mutex_lock(&verify_mtx);
    bool verified = set_contains(successful_rules, root) || set_contains(failed_rules, root);
    while(!verified) {
        pthread_cond_wait(&verify_cond, &verify_mtx);
        verified = set_contains(successful_rules, root) || set_contains(failed_rules, root);
    }
    pthread_mutex_unlock(&verify_mtx);
}

void *worker_thread(void *arg) {
    graph *dependency_graph = (graph *) arg;
    char *cur_node;
    while(true) {
        pthread_mutex_lock(&queue_mtx);
        while(queue_size == 0) {
            if (job_done) {
                pthread_mutex_unlock(&queue_mtx);
                pthread_exit(NULL);
            }
            pthread_cond_wait(&queue_cond, &queue_mtx);
        }
        queue_size--;
        pthread_mutex_unlock(&queue_mtx);
        cur_node = queue_pull(waiting_rules);

        // Check if the cur_node is the name of a file on the disk
        struct stat cur_data;
        bool is_file = stat(cur_node, &cur_data) == 0;
        bool children_are_files = true;
        bool child_is_younger = false;
        vector *cur_vect = graph_neighbors(dependency_graph, cur_node);
        size_t cur_size = vector_size(cur_vect);
        size_t i;
        if (is_file) {
            // Check if all of the cur_node's dependencies are the names of files on the disk
            if (cur_size == 0) {
                children_are_files = false;
            }
            for (i = 0; i < cur_size && children_are_files && !child_is_younger; i++) {
                struct stat child_data;
                children_are_files = children_are_files && (stat(vector_get(cur_vect, i), &child_data) == 0);
                // Check if a child file has a newer modification time than the current file
                if (children_are_files && (difftime(cur_data.st_mtime, child_data.st_mtime) < 0)) {
                    child_is_younger = true;
                }
            }
        }

        bool result = true;
        // Check if all of its children are successful
        for (i = 0; i < cur_size && result; i++) {
            char *child_node = vector_get(cur_vect, i);         // These should be shallow copies
            
            // Check if the root has been verified, and wait if it hasn't.
            pthread_mutex_lock(&verify_mtx);
            bool verified = set_contains(successful_rules, child_node) || set_contains(failed_rules, child_node);
            while(!verified) {
                pthread_cond_wait(&verify_cond, &verify_mtx);
                verified = set_contains(successful_rules, child_node) || set_contains(failed_rules, child_node);
            }
            pthread_mutex_unlock(&verify_mtx);
            // If the child rule failed, mark result as false
            bool failed = set_contains(failed_rules, child_node);
            if (failed) {
                result = false;
            }
        }
    
        // Upon completion, free cur_vect and its pointer array of neighbors
        free(vector_begin(cur_vect));
        free(cur_vect);
        cur_vect = NULL;

        // If result is false, then mark the cur_node as failed and continue to the next node
        if (!result) {
            set_add(failed_rules, cur_node);
            pthread_cond_broadcast(&verify_cond);
            continue;
        }

        // If the rule is not the name of a file, we always run it
        // If any of the parent file's dependencies are not a child, then we need to run the parent rule
        // If a child file was changed after the parent file, then we need to update the parent file
        if (!is_file || !children_are_files || child_is_younger) {
            rule_t *rule = (rule_t *)graph_get_vertex_value(dependency_graph, cur_node);
            vector *commands = rule->commands;
            size_t i;
            for (i = 0; i < vector_size(commands); i++) {
                if(system(vector_get(commands, i)) != 0) {
                    set_add(failed_rules, (void *) cur_node);
                    pthread_cond_broadcast(&verify_cond);
                    break;
                }
            }
            if (i == vector_size(commands)) {
                set_add(successful_rules, (void *) cur_node);
                pthread_cond_broadcast(&verify_cond);
            }
            rule = NULL;
            commands = NULL;
        } else {
            // If cur_data is a file, its children are files, and all of its children are older,
            // then the rule is already satisfied.
            set_add(successful_rules, (void *) cur_node);
            pthread_cond_broadcast(&verify_cond);
        }
    }
    pthread_exit(NULL);
}


int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    // Create a queue to store the rules that will be processed by the threads
    if (num_threads == 0) {
        graph *dependency_graph = parser_parse_makefile(makefile, targets);

        vector *vect_targets = graph_neighbors(dependency_graph, "");

        size_t i;
        for (i = 0; i < vector_size(vect_targets); i++) {
            char *target = vector_get(vect_targets, i);
            if (contains_cycle(dependency_graph, target)) {
                print_cycle_failure(target);
                continue;
            }
            run_target_threadless(dependency_graph, target);
        }

        graph_destroy(dependency_graph);
        dependency_graph = NULL;

        free(vector_begin(vect_targets));
        free(vect_targets);
        vect_targets = NULL;
    } else {
        // Initialize the mutex
        verify_mtx = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        verify_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

        // Initialize the dependency graph
        graph *dependency_graph = parser_parse_makefile(makefile, targets);

        // Initialize the queue
        waiting_rules = queue_create(-1);
        queue_size = 0;
        job_done = false;
        queue_mtx = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
        queue_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

        // Track all the rules
        failed_rules = string_set_create();
        successful_rules = string_set_create();
        all_rules = string_set_create();

        // Initialize the workers
        pthread_t *workers = calloc(num_threads, sizeof(pthread_t));
        size_t i;
        for (i = 0; i < num_threads; i++) {
            // Pass in the graph to each thread
            if (pthread_create(&(workers[i]), NULL, worker_thread, (void *) dependency_graph) != 0) {
                perror("pthread_create() error\n");
                exit(1);
            }
        }

        vector *vect_targets = graph_neighbors(dependency_graph, "");

        for (i = 0; i < vector_size(vect_targets); i++) {
            char *target = vector_get(vect_targets, i);
            if (contains_cycle(dependency_graph, target)) {
                print_cycle_failure(target);
                continue;
            }
            run_target(dependency_graph, target);
        }

        pthread_mutex_lock(&queue_mtx);
        job_done = true;
        pthread_cond_broadcast(&queue_cond);
        pthread_mutex_unlock(&queue_mtx);

        // Rejoin the threads once we're finished
        void *return_val;
        for (i = 0; i < num_threads; i++) {
            if (pthread_join(workers[i], &return_val) != 0) {
                perror("pthread_join() error\n");
                exit(1);
            }
        }
        
        // Cleanup
        free(workers);
        workers = NULL;
        pthread_mutex_destroy(&verify_mtx);
        pthread_cond_destroy(&verify_cond);

        pthread_mutex_destroy(&queue_mtx);
        pthread_cond_destroy(&queue_cond);

        queue_destroy(waiting_rules);
        waiting_rules = NULL;

        // Destroy the rule sets
        set_destroy(all_rules);
        all_rules = NULL;
        set_destroy(failed_rules);
        failed_rules = NULL;
        set_destroy(successful_rules);
        successful_rules = NULL;

        graph_destroy(dependency_graph);
        dependency_graph = NULL;

        free(vector_begin(vect_targets));
        free(vect_targets);
        vect_targets = NULL;
    }
    return 0;
}