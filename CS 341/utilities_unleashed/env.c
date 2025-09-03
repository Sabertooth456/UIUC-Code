/**
 * utilities_unleashed
 * CS 341 Spring - 2024
 */

// Code reused from Fall 2023

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include<wait.h>

#include<stdbool.h>

#include<string.h>

#include "format.h"

#include <ctype.h>

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
 * Creates a deep copy of the given array from index i = start to i < end
 * 
 * NOTE: You need to free the initialized memory that's returned by this function.
*/
char **subset_of_argv(char *argv[], int start, int end) {
    char **new_argv = (char **)malloc((end - start) * sizeof(char *));
    int i;
    for (i = start; i < end; i++) {
        new_argv[i - start] = calloc((strlen(argv[i]) + 1), sizeof(char));
        // printf("%zu\n", sizeof(char) * (strlen(argv[i]) + 1));
        strcpy(new_argv[i - start], argv[i]);
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

bool is_valid_arg(char *arg) {
    bool equality_found = false;
    size_t i;
    for (i = 0; i < strlen(arg); i++) {
        if (arg[i] == '=') {
            if (equality_found) {
                return false;
            } else {
                equality_found = true;
            }
        }
        else if (!isalnum(arg[i]) && arg[i] != '_') {
            return false;
        }
    }
    return equality_found;
}

// argc- Number of arguments, including the executable's name
// argv- a char array of those arguments
int main(int argc, char *argv[]) {
    // File call had too few arguments
    if (argc < 3) {
        print_env_usage();
    }
    // print_exec_args(argc, argv);
    int num_vars = 0;
    int num_args = 0;

    bool separator_found = false;   // separator = "--"
    bool command_found = false;     // The command follows right after the separator
    bool valid_args = true;         // Checks if the arguments have valid names
    char *cmd;

    int i;
    for (i = 1; i < argc; i++) {
        // Counting the variables
        if (!separator_found) {
            // Looking for the separator
            if (strcmp(argv[i], "--") == 0) {
                separator_found = true;
                continue;
            }
            // Counting the variables
            else {
                // Check if the variable is valid
                if (!is_valid_arg(argv[i])) {
                    valid_args = false;
                    break;
                }
                num_vars++;
            }
        }
        else {
            // Reading the command
            if (!command_found) {
                cmd = malloc(sizeof(char) * (strlen(argv[i]) + 1));
                strcpy(cmd, argv[i]);
                num_args++;
                command_found = true;
            }
            // Counting the arguments
            else {
                // Check if the variable is valid
                if (!is_valid_arg(argv[i])) {
                    valid_args = false;
                    break;
                }
                num_args++;
            }
        }
    }

    // File call invalid usage
    if (!valid_args || !separator_found || !command_found) {
        print_env_usage();
    }

    // printf("Command: %s\n", cmd);
    // printf("Variables: %d\n", num_vars);
    // printf("Arguments: %d\n", num_args);

    // Environment variables
    char **env_vars = subset_of_argv(argv, 1, 1 + num_vars);
    // Function arguments
    char **env_args = subset_of_argv(argv, 2 + num_vars, 2 + num_vars + num_args);

    // print_exec_args(num_vars, env_vars);
    // print_exec_args(num_args, env_args);

    pid_t pid = fork();
    // If fork() fails
    if (pid < -1) {
        print_fork_failed();
    }
    // If this fork is the child
    if (pid == 0) {
        // Not sure if we're supposed to use setenv() but oh well

        // Set the envrionment's variables
        for (i = 0; i < num_vars; i++) {
            // Create a buffer that will be brutalized by strtok()
            char *buffer = malloc(sizeof(char) * (strlen(env_vars[i]) + 1));
            strcpy(buffer, env_vars[i]);

            // Get the key
            char *token = strtok(buffer, "=");
            char *key = calloc((strlen(token) + 1), sizeof(char));
            strcpy(key, token);

            // Get the value
            token = strtok(NULL, "%");
            char *value = calloc((strlen(token) + 1), sizeof(char));
            strcpy(value, token);

            // If the user didn't pass in a "=" for their variable ("[key=val1]")
            if (value == NULL) {
                print_env_usage();
            }
            // printf("%s = %s\n", key, value);

            // Reset the buffer to look for "%"
            strcpy(buffer, env_vars[i]);

            // Start looking for "%""
            char delim[] = "%";
            token = strtok(buffer, delim);
            // printf("Token after 1 strtok %%: %s\n", token);
            token = strtok(NULL, delim);
            // printf("Token after 2 strtok %%: %s\n", token);

            // Else, replace all %KEYS with their VALUES
            while (token != NULL) {
                char *key_replacement = getenv(token);
                printf("getenv(value): %s\n", key_replacement);
                if (key_replacement == NULL) {
                    print_environment_change_failed();
                }
                value = realloc(value, (strlen(value) + (strlen(key_replacement) + 1)) * sizeof(char));
                strcat(value, key_replacement);
                token = strtok(NULL, delim);
            }

            // (key, value, overwrite)
            printf("%s = %s\n", key, value);
            int result = setenv(key, value, 1);
            if (result == -1) {
                print_environment_change_failed();
            }
        }

        // Run the command
        execvp(env_args[0], env_args);  // Fork bomb incoming xd
        print_exec_failed();
    }
    // Else, this fork is the parent
    else {
        int status;
        waitpid(pid, &status, 0);
        
        if (status != 0) {
            printf("Status: %d\n", status);
            print_exec_failed();
        }

        free(cmd);
        cmd = NULL;
        free_new_argv(num_vars, &env_vars);
        free_new_argv(num_args, &env_args);
    }

    return 0;
}