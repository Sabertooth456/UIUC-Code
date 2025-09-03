/**
 * mapreduce
 * CS 341 - Spring 2024
 */

// Code reused from Fall 2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <errno.h>

#include "utils.h"

#define CMD_LINE_MAX_SIZE (8191)

int main(int argc, char **argv) {
    if (argc != 6) {
        print_usage();
        return 0;
    }
    char *input_file = malloc(CMD_LINE_MAX_SIZE);
    strcpy(input_file, argv[1]);
    char *output_file = malloc(CMD_LINE_MAX_SIZE);
    strcpy(output_file, argv[2]);
    char *mapper_exec = malloc(CMD_LINE_MAX_SIZE);
    strcpy(mapper_exec, argv[3]);
    char *reducer_exec = malloc(CMD_LINE_MAX_SIZE);
    strcpy(reducer_exec, argv[4]);
    size_t num_mappers;
    if (sscanf(argv[5], "%zu", &num_mappers) != 1) {
        perror("mapreduce(): Invalid input for num_mappers.");
        exit(1);
    }

    // Create an input pipe for each mapper.
    int pipe_mapper[2];

    // Create one input pipe for the reducer.
    int pipe_reducer[2];
    pipe2(pipe_reducer, __O_CLOEXEC);

    // Open the output file.
    FILE *out = fopen(output_file, "w");

    // Start all the mapper processes.
    pid_t children[num_mappers];
    size_t segment_id = num_mappers;
    size_t i;
    for (i = 0; i < num_mappers; i++) {
        pid_t id = fork();
        // If the fork fails
        if (id == -1) {
            perror("fork() mapper process failed.\n");
            exit(1);
        }
        // If this is the child process
        else if (id == 0) {
            // Fork bomb time lmao
            segment_id = i;
            break;
        }
        // If this is the parent process, store the child's id and keep creating children
        children[i] = id;
    }

    // Start a splitter process for each mapper.
    if (segment_id != num_mappers) {
        pipe2(pipe_mapper, __O_CLOEXEC);
        pid_t child_id = fork();
        close(pipe_reducer[0]);
        if (child_id == -1) {     // fork() failure
            perror("fork() splitter process failed.\n");
            exit(1);
        } else if (child_id == 0) {   // Child process
            // Create the command
            char *splitter_args[5];
            splitter_args[0] = "./splitter";
            splitter_args[1] = malloc(CMD_LINE_MAX_SIZE);
            strcpy(splitter_args[1], input_file);\
            splitter_args[2] = malloc(CMD_LINE_MAX_SIZE);
            sprintf(splitter_args[2], "%zu", num_mappers);
            splitter_args[3] = malloc(CMD_LINE_MAX_SIZE);
            sprintf(splitter_args[3], "%zu", segment_id);
            splitter_args[4] = NULL;

            // Close the pipe's output and redirect all output into the pipe's input
            close(pipe_mapper[0]);
            dup2(pipe_mapper[1], 1);  // stdout

            // Run the command
            execv(splitter_args[0], splitter_args);
            print_nonzero_exit_status(splitter_args[0], errno);
            exit(1);
        } else {
            close(pipe_mapper[1]);
            int status;
            waitpid(child_id, &status, 0);
            if (status == -1) {
                perror("./splitter: status == -1\n");
                exit(1);
            }

            // Use the output of ./splitter as the input for the mapper and pipe the output of the mapper into the reducer
            dup2(pipe_mapper[0], STDIN_FILENO);    // stdin
            dup2(pipe_reducer[1], STDOUT_FILENO);   // stdout

            char *mapper_args[2];
            mapper_args[0] = malloc(CMD_LINE_MAX_SIZE);
            strcpy(mapper_args[0], mapper_exec);
            mapper_args[1] = NULL;
            execv(mapper_args[0], mapper_args);
            print_nonzero_exit_status(mapper_args[0], errno);
            exit(1);
        }
    }
    // The only remaining process should be the parent process
    // printf("I'm the parent process and this should only be printed once!\n");

    close(pipe_reducer[1]);
    // Wait for all of the children to finish working
    for (i = 0; i < num_mappers; i++) {
        int status;
        waitpid(children[i], &status, 0);
        if (status == -1) {
            perror("./mapper: status == -1\n");
            exit(1);
        }
    }
    // Start the reducer process.

    int pipe_final[2];
    pipe2(pipe_final, __O_CLOEXEC);
    pid_t child_id = fork();
    if (child_id == -1) {     // fork() failure
        perror("fork() splitter process failed.\n");
        exit(1);
    } else if (child_id == 0) {     // Child process
        // Use the output of pipe_reducer as the input for the reducer program, and send the program's output to pipe_final
        dup2(pipe_reducer[0], 0);    // stdin

        close(pipe_final[0]);
        dup2(pipe_final[1], 1);   // stdout
        
        char *reducer_args[2];
        reducer_args[0] = malloc(CMD_LINE_MAX_SIZE);
        strcpy(reducer_args[0], reducer_exec);
        reducer_args[1] = NULL;
        execv(reducer_args[0], reducer_args);
        print_nonzero_exit_status(reducer_args[0], errno);
        exit(1);
    }
    close(pipe_final[1]);

    // char buffer[CMD_LINE_MAX_SIZE];
    // read(pipe_reducer[0], buffer, sizeof(buffer));
    // printf("%s\n", buffer);

    // Wait for the reducer to finish.
    int status;
    waitpid(child_id, &status, 0);
    if (status == -1) {
        perror("./reducer: status == -1");
    }

    // Print nonzero subprocess exit codes.
    char buffer[CMD_LINE_MAX_SIZE];
    while(read(pipe_final[0], buffer, sizeof(buffer)) != 0) {
        fprintf(out, "%s", buffer);
    }

    // Count the number of lines in the output file.

    free(input_file);
    input_file = NULL;
    free(output_file);
    output_file = NULL;
    free(mapper_exec);
    mapper_exec = NULL;
    free(reducer_exec);
    reducer_exec = NULL;
    fclose(out);
    return 0;
}