/**
 * shell
 * CS 341 - Spring 2024
 */

// Code reused from Fall 2023

#include <stdio.h>      // stdin, fopen(), getline()
#include <unistd.h>     // getcwd(), getpid()
#include <wait.h>       // waitpid()
#include <string.h>     // Not sure if we're allowed to use this lol
#include <signal.h>     // signal(SIGINT, function)

#include "stdbool.h"    // bool

#include "format.h"
#include "shell.h"

#include "callbacks.h"
#include "sstring.h"
#include "vector.h"

typedef struct process {
    char *command;
    pid_t pid;
} process;

/**
 * Stores the index of each argument used to call the program
*/
typedef struct arguments {
    int read;
    int write;
} arguments;

#define CMD_LINE_MAX_LEN 8191

/**
 * Gets a single line from the command line, displaying the current 'directory' and pid 
 * 
 * Returns NULL when the End Of File is reached
 * 
 * NOTE: The returned string needs to be freed from memory
*/
char *get_line_user(const char *directory) {
    pid_t pid = getpid();
    char* str = malloc(CMD_LINE_MAX_LEN);    // Initialize a char array that can hold a string of length CMD_LINE_MAX_CHAR
    // fgets() returns NULL when it encounters an error or the user passes in an empty file
    void *result;
    do
    {
        print_prompt(directory, pid);
    } while ((result = fgets(str, CMD_LINE_MAX_LEN, stdin)) != NULL && strlen(str) <= 1);
    if (result == NULL) {
        free(str);
        printf("\n");       // Not sure if we're supposed to do this
        return NULL;
    }
    str[strcspn(str, "\n")] = '\0';

    // Return
    return str;
}

/**
 * Gets a single line from the command file, displaying the current 'directory' and pid 
 * 
 * Returns NULL when the End Of FIle is reached
 * 
 * NOTE: The returned string needs to be freed from memory
*/
char *get_line_command(const char *directory, vector *commands, const size_t command_index) {
    pid_t pid = getpid();
    char* str = malloc(CMD_LINE_MAX_LEN);    // Initialize a char array that can hold a string of length CMD_LINE_MAX_CHAR

    // If the end of the file has been reached
    size_t size = vector_size(commands);
    if (size == command_index) {
        free(str);
        str = NULL;
        return NULL;
    }

    // Print the prompt
    print_prompt(directory, pid);

    // Get the current line of the file and display it in the terminal
    str = strcpy(str, vector_get(commands, command_index));
    print_command(str);

    // Return
    return str;
}

/**
 * Gets a single line from the history vector
 * 
 * NOTE: The returned string needs to be freed from memory
*/
char *get_line_history(vector *history, const size_t history_index) {
    char* str = malloc(CMD_LINE_MAX_LEN);    // Initialize a char array that can hold a string of length CMD_LINE_MAX_CHAR

    // Get the specified index of the vector
    str = strcpy(str, vector_get(history, history_index));

    // Return
    return str;
}

/**
 * When you're so good that you use a vector as an sstring without reading the sstring.h file.
 * 
 * Returns a vector stored in heap memory that needs to be freed
 */
vector *process_line(const char *line) {
    // Create a vector that will store each word in the line
    vector *vec = malloc(sizeof(vector *));
    vec = string_vector_create();

    // Create a buffer to be brutalized by strtok
    char *buffer = malloc(CMD_LINE_MAX_LEN);
    buffer = strcpy(buffer, line);
    
    // Set the starting point of strtok
    char *token = NULL;
    token = strtok(buffer, " ");
    while(token != NULL) {
        vector_push_back(vec, token);
        token = strtok(NULL, " ");
    }

    // Free heap memory
    free(buffer);
    buffer = NULL;

    // Return
    return vec;
}

/**
 * Displays the contents of a string vector
*/
void print_string_vector(vector *vec) {
    size_t size = vector_size(vec);
    size_t i;
    for (i = 0; i < size-1; i++) {
        printf("%s, ", (char *)vector_get(vec, i));
    }
    if (size > 0) {
        printf("%s", (char *)vector_get(vec, size - 1));
    }
    printf("\n");
}

/**
 * Displays the contents of a vector of 'history'
*/
void print_history(vector *history) {
    size_t size = vector_size(history);
    size_t i;
    for (i = 0; i < size; i++) {
        char *cmd = vector_get(history, i);
        print_history_line(i, cmd);
    }
}

/**
 * Frees the 'dir', 'line', and 'cmd' variables initialized at the start of each loop.
 * 
 * Use NULL if the variable doesn't need to be freed
*/
void clean(char **dir, char **line, vector **cmd) {
    if (dir != NULL) {
        free(*dir);
        *dir = NULL;
    }
    if (line != NULL) {
        free(*line);
        *line = NULL;
    }
    if (cmd != NULL) {
        vector_destroy(*cmd);
        *cmd = NULL;
    }
}

/**
 * Converts a vector into a char array
*/
char **vector_to_char_array(vector *vec) {
    size_t size = vector_size(vec);
    char **char_array = (char **)malloc(sizeof(char *) * (size + 1));
    size_t i;
    for (i = 0; i < size; i++) {
        char *word = vector_get(vec, i);
        char_array[i] = malloc(sizeof(char) * (strlen(word) + 1));
        strcpy(char_array[i], word);
    }
    char_array[size] = NULL;
    return char_array;
}

/**
 * Frees the given char array 'arr'
*/
void free_char_array(size_t size, char **arr[]) {
    size_t i;
    for (i = 0; i < size; i++) {
        free((*arr)[i]);
        (*arr)[i] = NULL;
    }
    free(*arr);
    *arr = NULL;
}

/**
 * Yoinked from the CS341 textbook.
 * 
 * Cleans up background childprocesses when they finish
*/
void pid_cleanup(pid_t child) {
    // waidpid() returns 0 if WNOHANG is enabled and no child/children specified by id (-1) has changed state.
    // Effectively, this continuously calls waitpid() and WNOHANG allows it to run in the background
    while (waitpid(child, 0, WNOHANG) > 0) {

    }
}

/**
 * Executes the given external command, creating a fork if necessary.
 * 
 * Returns the exit status of the executed command
*/
int execute_line_external(char *line, vector *cmd, bool background_process) {
    char **cmd_array = vector_to_char_array(cmd);

    pid_t pid = fork();
    // If fork() fails
    if (pid < -1) {
        print_fork_failed();
    }
    // If this fork is the child
    if (pid == 0) {
        // char *dir = getcwd(NULL, 0);
        execvp(cmd_array[0], cmd_array);  // Fork bomb incoming
        print_exec_failed(line);
        exit(1);
    }
    // Else, this fork is the parent. pid stores the process id of the child process
    else {
        print_command_executed(pid);
        if (background_process) {
            int status;
            int result = waitpid(pid, &status, WNOHANG);
            if (result == -1) {
                print_wait_failed();
            }
            free_char_array(vector_size(cmd), &cmd_array);
            // Run the pid_cleanup() command to wait for the child process to finish
            pid_cleanup(pid);
            return status;
        } else {
            int status;
            int result = waitpid(pid, &status, 0);
            if (result == -1) {
                print_wait_failed();
            }
            free_char_array(vector_size(cmd), &cmd_array);
            return status;
        }
    }
}

/**
 * Change the current working directory to the given path.
 * Paths that don't start with '/' will be followed relative to the current directory
*/
void change_directory(vector *cmd) {
    // If there isn't a specified path
    if (vector_size(cmd) < 2) {
        print_no_directory("");
        return;
    }
    char *path = vector_get(cmd, 1);

    // Create a copy of the original name to use when printing errors
    char *original_path = malloc(CMD_LINE_MAX_LEN);
    original_path = strcpy(original_path, path);

    sstring *str_path = cstr_to_sstring(path);
    if (path[0] != '/') {
        sstring* rel_path = cstr_to_sstring("./");
        // Appends str_path to rel_path. The pointers need to be switched.
        sstring_append(rel_path, str_path);

        // Swap the pointers so that str_path now contains the new string
        sstring* temp = str_path;
        str_path = rel_path;
        rel_path = temp;
        
        // Free the old sstring variables
        sstring_destroy(rel_path);
        rel_path = NULL;
        temp = NULL;
    }
    char *new_path = sstring_to_cstr(str_path);
    int result = chdir(new_path);
    if (result == -1) {
        print_no_directory(original_path);
    }
    // Free the heap memory
    sstring_destroy(str_path);
    str_path = NULL;
    free(original_path);
    original_path = NULL;
    free(new_path);
    new_path = NULL;
}

bool valid_num(char *num) {
    size_t size = strlen(num);
    size_t i;
    for (i = 0; i < size; i++) {
        if (num[i] < '0' || num[i] > '9') {
            return false;
        }
    }
    return true;
}

int read_history_number(char *word, char *line, vector *history) {
    // Since we know that name starts with '#', it should be possible to increment
    // the pointer so that it looks at the following value
    char *num = word + 1;
    if (!valid_num(num)) {
        print_invalid_command(line);
        return -1;
    }
    size_t index = atoi(num);
    if (index >= vector_size(history)) {
        print_invalid_index();
        return -1;
    }
    return index;
}

/**
 * Returns true if 'word' contains 'prefix' and false otherwise.
*/
bool contains_prefix(char *prefix, char *line) {
    size_t prefix_length = strlen(prefix);
    size_t line_length = strlen(line);
    // If prefix is longer than the line
    if (line_length < prefix_length) {
        return false;
    }
    // Otherwised, check if the prefixes match
    size_t i;
    for (i = 0; i < prefix_length; i++) {
        if (line[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

/**
 * Searches the 'history' vector to see if it contains any commands with the given prefix (prefix is the pointer to word+1)
*/
int search_history(char *word, vector *history) {
    // Since we know that name starts with '1', it should be possible to increment
    // the pointer so that it looks at the following value
    char *prefix = word + 1;
    // Search through history for the last command that has the specified prefix

    size_t size = vector_size(history);

    // size_t can't store negative values, so i is 1-indexed instead of 0-indexed
    size_t i;
    for (i = size; i > 0; i--) {
        char *history_line = vector_get(history, i - 1);
        if (contains_prefix(prefix, history_line)) {
            return i - 1;
        }
    }
    print_no_history_match();
    return -1;
}

/**
 * Executes the given internal command.
 * Returns -1 on success/ invalid command.
 * Returns the index of the executable command 'n' on #<n> or !<prefix>.
*/
int execute_line_internal(char *line, vector *cmd, vector *history) {
    char *name = vector_get(cmd, 0);
    // cd <path>
    if (strcmp(name, "cd") == 0) {
        change_directory(cmd);
    }
    // !history
    else if (strcmp(name, "!history") == 0) {
        print_history(history);
    }
    // #<n>
    else if (name[0] == '#') {
        return read_history_number(name, line, history);
    }
    // !<prefix>
    else if (name[0] == '!') {
        return search_history(line, history);
    }
    return -1;
}

/**
 * Checks if the given command and returns whether or not the command be stored in the history
*/
bool is_memorable(vector *cmd) {
    char *name = vector_get(cmd, 0);
    if (strcmp(name, "exit") == 0 ||
        strcmp(name, "!history") == 0 ||
        name[0] == '#' ||   // NOTE: The #<n> command is NOT stored in history, but the command being executed (if any) IS.
        name[0] == '!'      // NOTE: The !<prefix> command itself is not stored in history, but the command being executed (if any) IS.
        ) {
        return false;
    }
    return true;
}

/**
 * Checks the given command and returns whether or not the command is an external one
*/
bool is_external_command(vector *cmd) {
    char *name = vector_get(cmd, 0);
    // These commands are built into the shell
    if (strcmp(name, "exit") == 0 ||
        strcmp(name, "cd") == 0 ||
        strcmp(name, "!history") == 0 ||
        name[0] == '#' ||
        name[0] == '!'
        ) {
            return false;
    }
    return true;
}

/**
 * Checks if the initial execution of the file included a '-f' followed by a 'file' argument
 * 
 * If true, returns the index of the 'file'
 */
int contains_read_arg(const int argc, char *argv[]) {
    if (argc == 3) {
        if (strcmp(argv[1], "-f") == 0) {
            return 2;
        }
    }
    if (argc == 5) {
        if (strcmp(argv[1], "-f") == 0) {
            return 2;
        } else if (strcmp(argv[3], "-f") == 0) {
            return 4;
        }
    }
    return -1;
}

/**
 * Checks if the initial execution of the file included a '-h' followed by a 'file' argument
 * 
 * If true, returns the index of the 'file'
 * If false, returns -1
 */
int contains_write_arg(const int argc, char *argv[]) {
    if (argc == 3) {
        if (strcmp(argv[1], "-h") == 0) {
            return 2;
        }
    }
    if (argc == 5) {
        if (strcmp(argv[1], "-h") == 0) {
            return 2;
        } else if (strcmp(argv[3], "-h") == 0) {
            return 4;
        }
    }
    return -1;
}

/**
 * Stores the parsed arguments into a arguments struct
*/
arguments parse_args(const int argc, char *argv[]) {
    arguments args;
    args.read = contains_read_arg(argc, argv);
    args.write = contains_write_arg(argc, argv);
    return args;
}

/**
 * Checks if the user provided a valid set of arguments
*/
bool valid_args(const int argc, const arguments args) {
    // NOTE: argc == 1 when given no arguments because the name of the function is also passed into argv.
    // 0 arg
    if (argc == 1) {
        // Slight redundancy, but I think it's a valid idiot check
        return ((args.read == -1) && (args.write == -1));
    }
    // 1 arg
    else if (argc == 3) {
        return ((args.read != -1 && args.write == -1) || (args.read == -1 && args.write != -1));
    }
    // 2 arg
    else if (argc == 5) {
        return ((args.read != -1) && (args.write != -1) && (args.read != args.write));
    }
    return false;
} 

/**
 * Reads the given 'fptr' file and sequentially inserts each line into the 'file' vector
*/
void download_file(FILE* fptr, vector *vec) {
    char *line = malloc(CMD_LINE_MAX_LEN);
    size_t size = CMD_LINE_MAX_LEN;
    int result;
    do {
        // getline() includes the '\n'
        result = getline(&line, &size, fptr);
        // Continue if an empty line is read
        if (result != -1) {
            line[strcspn(line, "\n")] = '\0';    // Remove the '\n'
            vector_push_back(vec, line);
        }
    } while (result != -1);
    free(line);
    line = NULL;
}

void download_history(char **path, vector *vec) {
    FILE *fptr = malloc(sizeof(FILE *));
    // If fopen() fails to open 'path', print the error, free the heap memory, and exit
    fptr = fopen(*path, "r");
    // If the file does not exist, treat it as an empty file
    if (fptr == NULL) {
        return;
    }
    download_file(fptr, vec);
    fclose(fptr);
    fptr = NULL;
}

void download_script(char **path, vector *vec) {
    FILE *fptr = malloc(sizeof(FILE *));
    // If fopen() fails to open 'path', print the error, free the heap memory, and exit
    fptr = fopen(*path, "r");
    if (fptr == NULL) {
        print_script_file_error();
        vector_destroy(vec);
        vec = NULL;
        free(*path);
        *path = NULL;
        exit(1);
    }
    download_file(fptr, vec);
    fclose(fptr);
    fptr = NULL;
}

/**
 * Reads the given 'history' vector and sequentially inserts each line into the 'fptr' history
*/
void upload_history(vector *history, FILE* fptr) {
    char *line;
    size_t size = vector_size(history);
    size_t i;
    for (i = 0; i < size; i++) {
        line = vector_get(history, i);
        fprintf(fptr, "%s\n", line);
    }
}

void upload(char **path, vector *history) {
    FILE *fptr = NULL;
    // If fopen() fails to open 'path', print the error, free the heap memory, and exit
    if ((fptr = fopen(*path, "w")) == NULL) {
        print_history_file_error();
        vector_destroy(history);
        history = NULL;
        free(*path);
        *path = NULL;
        exit(1);
    }
    upload_history(history, fptr);
    fclose(fptr);
    fptr = NULL;
}

void SIGINT_handler(int val) {
    (void)(val);
}

/**
 * If the command contains a logical operator, returns a string literal equal to the operator. The string does not need to be freed.
 * 
 * Otherwise, returns NULL
*/
char *contains_operator(vector* command) {
    size_t size = vector_size(command);
    size_t i;
    for (i = 0; i < size; i++) {
        char *word = vector_get(command, i);
        size_t word_length = strlen(word);
        // && operator
        if (strcmp(word, "&&") == 0) {
            return "&&";
        }
        // || operator
        else if (strcmp(word, "||") == 0) {
            return "||";
        }
        // ; operator
        else if (word[word_length - 1] == ';') {
            return ";";
        }
    }
    return NULL;
}

/**
 * Executes the given command, whether it is an internal or external command.
 * 
 * Returns the exit status of the command, usually 0 on success and 1 on failure.
*/
int execute_line(char **line, vector *cmd, vector *history, bool *history_command, bool background_process) {
    fflush(stdout);

    // If the command is an external command
    if (is_external_command(cmd)) {
        return execute_line_external(*line, cmd, background_process);
    }
    // If the command is an internal command
    else {
        int result = execute_line_internal(*line, cmd, history);
        if (result != -1) {
            clean(NULL, line, &cmd);

            // Re-execute the loop with the new command
            *line = get_line_history(history, result);
            print_command(*line);
            *history_command = true;
        }
        return 0;
    }
}

/**
 * Given a specified operator, reallocates 'line' and 'cmd' to the first command and returns a char* to the second command.
 * 
 * There will be errors if passed an invalid operator.
*/
char *get_second_line(char *operator, char **line, vector **cmd) {
    sstring *string_line = NULL;
    vector *split_line = NULL;
    // Split the line into two commands
    string_line = cstr_to_sstring(*line);
    split_line = sstring_split(string_line, operator[0]);
    // Clean the old line and cmd
    clean(NULL, line, cmd);
    // Reinitialize line and cmd
    *line = malloc(CMD_LINE_MAX_LEN);
    *line = strcpy(*line, vector_get(split_line, 0));
    *cmd = process_line(*line);
    // Create the second line to be returned
    char *second_line = malloc(CMD_LINE_MAX_LEN);
    if ((strcmp(operator, "&&") == 0) || (strcmp(operator, "||") == 0)) {
        second_line = strcpy(second_line, vector_get(split_line, 2));
    } else if (strcmp(operator, ";") == 0) {
        second_line = strcpy(second_line, vector_get(split_line, 1));
    }
    // Clean the created vectors
    sstring_destroy(string_line);
    string_line = NULL;
    vector_destroy(split_line);
    split_line = NULL;
    return second_line;
}

/**
 * Compartmentalization magic. Given the 'exit_status', 'operator', and addresses of 'second_line', 'line', and 'cmd',
 * it updates the values in 'line' and 'cmd', and returns true if the second command should be executed. Otherwise, returns false.
*/
bool should_loop(int exit_status, char *operator, char **second_line, char **line, vector **cmd) {
    // Replace line with the command stored in second_line
    clean(NULL, line, cmd);
    *line = *second_line;
    *second_line = NULL;
    *cmd = process_line(*line);
    // Operator should not need to be initialized to NULL
    // AND operator
    if ((exit_status == 0) && (strcmp(operator, "&&") == 0)) {
        // If command exited successfully, run the second command
        return true;
    }
    // OR operator
    else if ((exit_status != 0) && (strcmp(operator, "||") == 0)) {
        // If the command did not exit successfully, run the second command
        return true;
    }
    // BOTH operator
    else if (strcmp(operator, ";") == 0) {
        return true;
    }
    return false;
}

int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    // Signal handling
    signal(SIGINT, SIGINT_handler);
    // 

    arguments args = parse_args(argc, argv);
    if (!valid_args(argc, args)) {
        print_usage();
        exit(1);
    }

    vector *history = malloc(sizeof(vector *));
    history = string_vector_create();
    char *history_path = NULL;
    // If a filename was specified to use for the history
    if (args.write != -1) {
        history_path = get_full_path(argv[args.write]);
        download_history(&history_path, history);
    }

    vector *commands = malloc(sizeof(vector *));
    commands = string_vector_create();
    char *command_path = NULL;
    size_t command_index = 0;
    // If a filename was specified to use for the commands
    if (args.read != -1) {
        command_path = get_full_path(argv[args.read]);
        download_script(&command_path, commands);
    }

    bool exit_received = false;

    // Main shell loop
    while(!exit_received) {
        char *dir = getcwd(NULL, 0);
        char *line;
        vector *cmd;
        bool record = true;

        // If there is no '-f' tag
        if (args.read == -1) {
            line = get_line_user(dir);
        }
        // If there is an '-f' tag
        else {
            line = get_line_command(dir, commands, command_index);
            command_index++;
        }

        if (line == NULL) {
            clean(&dir, NULL, NULL);
            break;
        }

        cmd = process_line(line);

        // "Use of a goto statement is highly discouraged in any programming language"
        // -https://www.tutorialspoint.com/cprogramming/c_goto_statement.htm
        NEW_EXEC:

        // Break if the cmd is "exit"
        if (strcmp(vector_get(cmd, 0), "exit") == 0) {
            clean(&dir, &line, &cmd);
            break;
        }

        // Check if the command requests a background process
        bool background_process = false;
        if (strcmp(vector_get(cmd, vector_size(cmd) - 1), "&") == 0) {
            background_process = true;
            vector_pop_back(cmd);
        }

        // If the command should be recorded
        if (record && is_memorable(cmd)) {
            vector_push_back(history, line);
        }

        // Check if the command contains a logical operator
        char *second_line = NULL;
        char *operator = contains_operator(cmd);
        // If the command contains an operator
        if (operator != NULL) {
            second_line = get_second_line(operator, &line, &cmd);
        }

        // There are some headaches to be had if history commands worked with logical operators. Fortunately, they don't.
        bool history_command = false;
        int exit_status = execute_line(&line, cmd, history, &history_command, background_process);
        if (history_command) {
            cmd = process_line(line);
            goto NEW_EXEC;
        }
        // Also fortunately, we don't have to write code for chaining operators
        else if (second_line != NULL) {
            if (should_loop(exit_status, operator, &second_line, &line, &cmd)) {
                goto NEW_EXEC;
            }
        }
        clean(&dir, &line, &cmd);
    }

    // If a filename was specified to use as the history
    if (args.write != -1) {
        upload(&history_path, history);
    }

    // Free heap memory
    vector_destroy(history);
    history = NULL;
    free(history_path);
    history_path = NULL;

    vector_destroy(commands);
    commands = NULL;
    free(command_path);
    command_path = NULL;
    exit(0);
}

/**
 * Part 1:
 *  Starting up a shell
 *  Optional arguments when launching a shell
 *  Interaction
 *  Built-in commands
 *  Foreground external commands
 *  Logical operators
 *  SIGINT handling
 *  Exiting
*/