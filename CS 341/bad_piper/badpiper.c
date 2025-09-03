// author: ytlin4
// Compiling command: clang -o badpiper -O0 -Wall -Wextra -Werror -g -std=c99 -D_GNU_SOURCE -DDEBUG badpiper.c

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>    // Bool

#include <sys/stat.h>   // stat
#include <fcntl.h>      // open, pipe

#include <unistd.h>     // access, dup2, pipe

#include <sys/wait.h>   // wait, waitpid

#include <string.h>     // strings stuff

#include <errno.h>      // errno

static char *password_path;

// Check that the correct number of arguments have been used
void validate_args(int argc) {
    // If 3 arguments are not specified, display a usage message on stderr and exit with a value of 1.
    if (argc != 4) {
        perror("./badpiper <file> <cmd> <cmd>\n");
        exit(1);
    }
}

// Yoinked from charming_chatroom lmao
ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    ssize_t bytes_read = 0;
    while((size_t) bytes_read < count) {
        ssize_t cur_read = read(socket, buffer + bytes_read, count - bytes_read);
        if (cur_read == 0) {
            return bytes_read;
        }
        else if (cur_read > 0) {
            bytes_read += cur_read;
        }
        else if (cur_read == -1 && errno == EINTR) {
            continue;
        }
        else {
            return -1;
        }
    }
    return bytes_read;
}

// ChatGPT <3
void replace_digits_with_asterisks(char *str, ssize_t bytes) {
    for (ssize_t i = 0; i < bytes; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            str[i] = '*';
        }
    }
}

// Uses up to 999 bytes from buf as the stdin stream for the second command. Pipes the output into the parent pipe
void second_child(int *parent_pipe, char **argv, char *buf, ssize_t bytes) {
    // Also stores these output bytes in a new file, $HOME/mypassword
    int password_fd = open(password_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    write(password_fd, buf, bytes);
    close(password_fd);
    
    // Open the temporary file as read only
    password_fd = open(password_path, O_RDONLY);
    close(parent_pipe[0]); // Close read end of pipe 2
    dup2(password_fd, STDIN_FILENO); // Redirect stdin to input file
    dup2(parent_pipe[1], STDOUT_FILENO); // Redirect stdout to write end of pipe 2
    close(parent_pipe[1]); // Close write end of pipe 2
    close(password_fd);

    free(buf);

    // Execute the command
    execlp(argv[3], argv[3], NULL);
    perror("Error\n");
    exit(2);
}

// Pipe the output of the first command into the second command
void first_child(int *parent_pipe, char **argv) {
    char *rel_path = argv[1];
    fflush(stdout);

    // Sends all of the input file contents as stdin stream to the first command, except...
    // Sends 999B from /dev/urandom if the input file does not exist, or...
    // Sends 999B from /etc/services if the input file exists but cannot be opened for reading.
    size_t file_size;
    struct stat info;
    if (stat(rel_path, &info) != 0) {
        rel_path = "/dev/urandom";
        file_size = 999;
    } else if (access(rel_path, R_OK) != 0) {
        rel_path = "/etc/services";
        file_size = 999;
    } else {
        file_size = info.st_size;
    }
    
    // Open the input file
    int input_file = open(rel_path, O_RDONLY); 
    if (input_file == -1) {    // This error shouldn't be possible
        perror("first_child, open(rel_path): ");
        exit(1);
    }

    // Transfer the bytes from the input file (or 999 bytes from /dev/urandom or /etc/services) to a buffer
    char *buf = calloc(1, file_size + 1);
    read(input_file, buf, file_size);
    close(input_file);

    // Write the contents of the input file to a temporary file, $HOME/mypassword, and then close it
    int output_file = open(password_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    write(output_file, buf, file_size);
    close(output_file);
    free(buf);
    buf = NULL;

    // Open the temporary file as read only
    input_file = open(password_path, O_RDONLY);
    close(parent_pipe[0]); // Close read end of pipe 1
    dup2(input_file, STDIN_FILENO); // Redirect stdin to input file
    dup2(parent_pipe[1], STDOUT_FILENO); // Redirect stdout to write end of pipe 1
    close(parent_pipe[1]); // Close write end of pipe 1
    close(input_file);

    // Execute the command
    execlp(argv[2], argv[2], NULL);
    perror("Error\n");
    exit(2);
}

int main(int argc, char **argv) {
    validate_args(argc);

    // Initialize the password path
    char *file_name = "/mypassword";
    password_path = malloc((size_t) (strlen(getenv("HOME")) + strlen(file_name) + 1)); // to account for NULL terminator
    strcpy(password_path, getenv("HOME"));
    strcat(password_path, file_name);
    
    // Create pipe for first command
    int pipe1[2];
    if (pipe(pipe1) == -1) {
        perror("Pipe 1 failed");
        exit(2);
    }

    // Fork for the first command
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Fork 1 failed");
        exit(2);
    }

    // Child process
    if (pid1 == 0) {
        first_child(pipe1, argv);
        exit(2);    // This should never happen
    }
    // Parent things
    close(pipe1[1]); // Close write end of pipe 1
    int status;
    waitpid(pid1, &status, 0);
    if (status == 2) {
        exit(2);
    }

    // Test command: ./badpiper rw_perms.txt echo fail
    // I have been debugging this for 3 hours and "echo" still fails to print to the pipe
    // Apparently I shouldn't have been using echo... fuck pipes

    // Uses up to the first 999B of output from the first command as the stdin stream for the second command.
    char *buf = calloc(1, 1000);
    ssize_t bytes = read_all_from_socket(pipe1[0], buf, 999);
    
    // Create pipe for second command
    int pipe2[2];
    if (pipe(pipe2) == -1) {
        perror("Pipe 2 failed");
        exit(2);
    }

    // Fork for the second command
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Fork 2 failed");
        exit(2);
    }

    // Child process
    if (pid2 == 0) {
        second_child(pipe2, argv, buf, bytes);
        exit(2);    // This should never happen
    }
    // Parent things
    close(pipe2[1]); // Close write end of pipe 2
    waitpid(pid2, &status, 0);
    if (status == 2) {
        exit(2);
    }

    // Displays the entire output of the second command (without any limit) but with digits replaced with an asterisk
    while ((bytes = read_all_from_socket(pipe2[0], buf, 999)) != 0) {
        replace_digits_with_asterisks(buf, bytes);
        fwrite(buf, 1, bytes, stdout);
    }

    // Closes anonymous POSIX pipes for input whenever there will be no more input for that pipe.
    close(pipe1[0]);
    close(pipe2[0]);

    // Cleanup
    free(buf);
    buf = NULL;
    free(password_path);
    password_path = NULL;

    // If either or both commands cannot be executed then exit with value 2, otherwise exit with value 0.
    exit(0);
}