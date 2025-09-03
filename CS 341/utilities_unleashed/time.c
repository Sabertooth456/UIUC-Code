/**
 * utilities_unleashed
 * CS 341 - Spring 2024
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include<time.h>
#include<wait.h>

#include<errno.h>

#include<string.h>

#include "format.h"

#define NANOSECONDS_PER_SECOND 1000000000

void print_exec_args(int argc, char *argv[]) {
    printf("\n");
    printf("argc: %i", argc);
    int i;
    for (i = 0; i < argc; i++) {
        printf("\narg%d: %s", i, argv[i]);
    }
    printf("\n\n");
}

/**
 * Creates a deep copy of the given array without the first element.
 * 
 * NOTE: You need to free the initialized memory that's returned by this function.
*/
char **pop_front_argv(int argc, char *argv[]) {
    char **new_argv = (char **)malloc((argc - 1) * sizeof(char *));
    int i;
    for (i = 1; i < argc; i++) {
        new_argv[i - 1] = malloc(sizeof(char) * (strlen(argv[i]) + 1));
        // printf("%zu\n", sizeof(char) * (strlen(argv[i]) + 1));
        strcpy(new_argv[i - 1], argv[i]);
    }
    return new_argv;
}

/**
 * Frees the memory allocated by the new argv string array and sets the array to NULL.
 * 
 * Usage: free_new_argv(argc, &argv) 
*/
void free_new_argv(int argc, char** argv[]) {
    int i;
    for (i = 0; i < argc; i++) {
        free((*argv)[i]);
        (*argv)[i] = NULL;
    }
    free(*argv);
    *argv = NULL;
}

// argc- Number of arguments, including the executable's name
// argv- a char array of those arguments
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_time_usage();
    }
    // print_exec_args(argc, argv);

    int cmd_argc = argc - 1;
    char **cmd_argv = pop_front_argv(argc, argv);

    // print_exec_args(cmd_argc, cmd_argv);

    pid_t pid = fork();
    // If fork() fails
    if (pid < -1) {
        print_fork_failed();
    }
    // If this fork is the child
    if (pid == 0) {
        execvp(cmd_argv[0], cmd_argv);  // Fork bomb incoming
        print_exec_failed();
    }
    // Else, this fork is the parent
    else {        
        // Get the starting time
        struct timespec *start = malloc(sizeof(struct timespec));
        clock_gettime(CLOCK_MONOTONIC, start);
        
        int status;
        waitpid(pid, &status, 0);

        // Get the ending time
        struct timespec *end = malloc(sizeof(struct timespec));
        clock_gettime(CLOCK_MONOTONIC, end);

        // printf("Start Sec: %ld, Start Nanosec: %ld\n", start->tv_sec, start->tv_nsec);
        // printf("End Sec: %ld, End Nanosec: %ld\n", end->tv_sec, end->tv_nsec);

        struct timespec *time_diff = malloc(sizeof(struct timespec));
        time_diff->tv_sec = end->tv_sec - start->tv_sec;
        time_diff->tv_nsec = end->tv_nsec - start->tv_nsec;
        if (time_diff->tv_nsec < 0) {
            time_diff->tv_sec -= 1;
            time_diff->tv_nsec += NANOSECONDS_PER_SECOND;
        }

        // printf("Sec: %ld, Nanosec: %ld\n", time_diff->tv_sec, time_diff->tv_nsec);
        double duration = (double)time_diff->tv_sec + ((double)time_diff->tv_nsec / NANOSECONDS_PER_SECOND);
        // printf("Total time: %lf\n", dec_time_diff);

        // printf("Status: %d\n", status);
        if (status == 0) { 
            display_results(argv, duration);
        }

        // Cleanup
        free(start);
        start = NULL;
        free(end);
        end = NULL;
        free(time_diff);
        time_diff = NULL;
        free_new_argv(cmd_argc, &cmd_argv);
    }
    return 0;
}