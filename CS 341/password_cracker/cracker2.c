/**
 * password_cracker
 * CS 341 - Spring 2024
 */
// Code reused from Fall 2023

#include "cracker2.h"
#include "libs/format.h"
#include "libs/utils.h"
#include "includes/queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>

#include <stdbool.h>
#include <string.h>

#include <pthread.h>

static pthread_mutex_t thread_assignment_mtx;
static pthread_mutex_t mtx;

#define SALT ("xx")
#define MAX_LENGTH (4096)

typedef struct password_data {
    size_t group_id;
    size_t thread_id;
    char *username;             // The account's username.
    char *known_chars;          // The known characters of the password, it is always at the start of the password.
    int unknown_length;      // The total unknown characters that need to be guessed
    char *target_hash;          // The hash that results when the correct password and salt is used.
    size_t available_threads;   // The threads available to split the work on cracking the password.
} password_data;

typedef struct queue_and_id {
    queue *password_queue;
    size_t thread_id;
} queue_and_id;

/**
 * Takes the address of 'guess_cur', 'guess_end', and 'target_hash_safe', frees them, and sets them to NULL.
*/
void clean(char **username, char **guess_cur, char **target_hash) {
    free(*username);
    *username = NULL;
    free(*guess_cur);
    *guess_cur = NULL;
    free(*target_hash);
    target_hash = NULL;
}

/**
 * Downloads the given data and initializes the copied values onto the heap.
*/
void download(password_data *data, size_t *thread_id, size_t *group_id, size_t *known_length, size_t *total_length,
        long *required_guesses, char **username, char **guess_cur, char **target_hash) {
    if (data->username == NULL) {
        perror("download(): data->username == NULL\n");
    }
    if (data->known_chars == NULL) {
        perror("download(): data->known_chars == NULL\n");
        exit(1);
    } else if (data->target_hash == NULL) {
        perror("download(): data->target_hash == NULL\n");
        exit(1);
    }
    *thread_id = data->thread_id;
    *group_id = data->group_id;

    // Copy over the pointer arguments before they are overwritten by another thread
    *username = malloc(strlen(data->username) + 1);
    *username = strcpy(*username, data->username);
    if (*username == NULL) {
        perror("download(): strcpy(username) == NULL\n");
    }

    *known_length = strlen(data->known_chars);
    *total_length = strlen(data->known_chars) + data->unknown_length;

    long start;
    getSubrange(data->unknown_length, data->available_threads, data->group_id, &start, required_guesses);
    *guess_cur = calloc(1, *total_length + 1);
    size_t i;
    for (i = 0; i < *known_length; i++) {
        (*guess_cur)[i] = data->known_chars[i];
    }

    char *unknown_start = malloc(data->unknown_length + 1);
    for (i = 0; (int)i < data->unknown_length; i++) {
        unknown_start[i] = 'a';
    }
    unknown_start[data->unknown_length] = '\0';
    setStringPosition(unknown_start, start);
    for (i = *known_length; i < *total_length; i++) {
        (*guess_cur)[i] = unknown_start[i - *known_length];
    }
    *target_hash = malloc(strlen(data->target_hash) + 1);
    *target_hash = strcpy(*target_hash, data->target_hash);
    if (*target_hash == NULL) {
        perror("download(): strcpy(target_hash) == NULL\n");
        exit(1);
    }
    // Clean up local heap variables
    free(unknown_start);
    unknown_start = NULL;
}

/**
 * Guesses the password given a password's known characters, unknown characters, the target hash, salt, and an optional start and end.
 * Prints the result.
 * 
 * @param data All of the data required to find a possible password
*/
void password_guesser(password_data *data) {
    // Download the values from data to prevent the pointers from being overwritten by another thread
    size_t thread_id; size_t group_id; size_t known_length; size_t total_length; long required_guesses;
    char *username = NULL; char *guess_cur = NULL; char *target_hash = NULL;

    download(data, &thread_id, &group_id, &known_length, &total_length, &required_guesses, &username, &guess_cur, &target_hash);

    v1_print_thread_start(thread_id, username);

    struct crypt_data cdata;
    cdata.initialized = 0;

    long i;
    for (i = 0; i < required_guesses; i++) {
        crypt_r(guess_cur, SALT, &cdata);
        const char *hash = cdata.output;
        if (hash == NULL) {
            perror("password_guesser(): crypt_r() returned NULL\n");
            exit(1);
        }
        if (strcmp(hash, target_hash) == 0) {
            v1_print_thread_result(thread_id, username, guess_cur, i, getThreadCPUTime(), 0);
            clean(&username, &guess_cur, &target_hash);
            return;
        }
        if (incrementString(guess_cur) != 1) {
            perror("password_guesser(): required_guesses is larger than the total possible guesses\n");
            exit(1);
        }
    }
    v1_print_thread_result(thread_id, username, guess_cur, i, getThreadCPUTime(), 1);
    clean(&username, &guess_cur, &target_hash);
}

/**
 * Plan:
 * Each password have possible answers split among each thread. This distribution will be determined by each thread's ID.
*/

void *run_thread(void *arg) {
    // Unpack the values
    queue_and_id *qai = (queue_and_id *)arg;
    queue *password_queue = qai->password_queue;
    size_t thread_id = qai->thread_id;
    pthread_mutex_unlock(&thread_assignment_mtx);

    while(true) {
        pthread_mutex_lock(&mtx);
        char *line = (char *)queue_pull(password_queue);
        if (strcmp(line, "NULL") == 0) {
            queue_push(password_queue, "NULL");
            pthread_mutex_unlock(&mtx);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mtx);
        password_data *data = malloc(sizeof(password_data));
        data->thread_id = thread_id;
        data->group_id = 1;
        data->available_threads = 1;
        data->username = strtok(line, " ");
        data->target_hash = strtok(NULL, " ");

        char *password = strtok(NULL, " ");
        size_t total_length = strlen(password);

        data->known_chars = strtok(password, ".");
        data->unknown_length = total_length - strlen(data->known_chars);
        
        password_guesser(data);

        free(data);
        data = NULL;
        free(line); // PROBLEM
        line = NULL;
    }
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads

    // Initialize a queue to store all passwords that need to be found
    queue *password_queue = queue_create(-1);
    char *input;
    int total_passwords;
    for (total_passwords = 0; total_passwords < 10000; total_passwords++) {
        input = malloc(MAX_LENGTH);
        char *result = fgets(input, MAX_LENGTH-1, stdin);
        if (result == NULL) {
            break;
        }
        // Since fgets includes the newline at the end of the line, we need to replace it with a null (end of string) pointer
        input[strlen(input) - 1] = '\0';
        queue_push(password_queue, (void *)input);
    }
    queue_push(password_queue, (void *)"NULL");

    mtx = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    thread_assignment_mtx = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

    // Initialize pthread_t
    pthread_t *threads = calloc(thread_count, sizeof(pthread_t));
    
    queue_and_id *qai = malloc(sizeof(queue_and_id));
    qai->password_queue = password_queue;
    qai->thread_id = 0;

    size_t i;
    for (i = 0; i < thread_count; i++) {
        // Create a new thread
        pthread_mutex_lock(&thread_assignment_mtx);
        qai->thread_id = i + 1;
        if (pthread_create(&(threads[i]), NULL, run_thread, qai) != 0) {
            perror("pthread_create() error\n");
            exit(1);
        }
    }

    // Rejoin the threads in another for loop
    void *return_val;
    for (i = 0; i < thread_count; i++) {
        if (pthread_join(threads[i], &return_val) != 0) {
            perror("pthread_join() error\n");
            exit(1);
        }
    }

    // Free stuff
    queue_destroy(password_queue);
    pthread_mutex_destroy(&mtx);
    pthread_mutex_destroy(&thread_assignment_mtx);
    free(input);
    input = NULL;
    free(threads);
    threads = NULL;
    free(qai);
    qai = NULL;

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}