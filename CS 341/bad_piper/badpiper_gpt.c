// author: ytlin4
// Compiling command: clang -o badpiper -O0 -Wall -Wextra -Werror -g -std=c99 -D_GNU_SOURCE -DDEBUG badpiper.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 1000

void replace_digits_with_asterisks(char *str) {
    for (char *ptr = str; *ptr != '\0'; ++ptr) {
        if (*ptr >= '0' && *ptr <= '9') {
            *ptr = '*';
        }
    }
}

int main(int argc, char *argv[]) {
    // Check argument count
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_filename> <command1> <command2>\n", argv[0]);
        return 1;
    }

    // Open input file or fallback to /dev/urandom or /etc/services
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        input_fd = open("/dev/urandom", O_RDONLY);
        if (input_fd == -1) {
            input_fd = open("/etc/services", O_RDONLY);
            if (input_fd == -1) {
                perror("Failed to open input file");
                return 2;
            }
        }
    }

    // Create pipe for first command
    int pipe1[2];
    if (pipe(pipe1) == -1) {
        perror("Pipe 1 failed");
        return 2;
    }

    // Fork for first command
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Fork 1 failed");
        return 2;
    }

    if (pid1 == 0) { // Child process for first command
        close(pipe1[0]); // Close read end of pipe 1
        dup2(input_fd, STDIN_FILENO); // Redirect stdin to input file
        dup2(pipe1[1], STDOUT_FILENO); // Redirect stdout to write end of pipe 1
        close(pipe1[1]); // Close write end of pipe 1

        // Execute first command
        execlp(argv[2], argv[2], NULL);
        perror("Failed to execute command 1");
        exit(2);
    } else { // Parent process
        close(pipe1[1]); // Close write end of pipe 1
        close(input_fd); // Close input file descriptor

        int status;
        waitpid(pid1, &status, 0);
        if (status == 2) {
            exit(2);
        }

        // Create pipe for second command
        int pipe2[2];
        if (pipe(pipe2) == -1) {
            perror("Pipe 2 failed");
            return 2;
        }

        // Fork for second command
        pid_t pid2 = fork();
        if (pid2 == -1) {
            perror("Fork 2 failed");
            return 2;
        }

        if (pid2 == 0) { // Child process for second command
            close(pipe2[0]); // Close read end of pipe 2
            dup2(pipe1[0], STDIN_FILENO); // Redirect stdin to read end of pipe 1
            dup2(pipe2[1], STDOUT_FILENO); // Redirect stdout to write end of pipe 2
            close(pipe1[0]); // Close read end of pipe 1
            close(pipe2[1]); // Close write end of pipe 2

            // Execute second command
            execlp(argv[3], argv[3], NULL);
            perror("Failed to execute command 2");
            exit(2);
        } else { // Parent process
            close(pipe1[0]); // Close read end of pipe 1

            // Read output of first command from pipe 1
            char buffer[BUF_SIZE];
            ssize_t read_bytes = read(pipe2[0], buffer, BUF_SIZE);
            if (read_bytes == -1) {
                perror("Failed to read from pipe 2");
                return 2;
            }

            // Write to mypassword file
            char *home_dir = getenv("HOME");
            if (home_dir == NULL) {
                fprintf(stderr, "Failed to get HOME environment variable\n");
                return 2;
            }
            char password_file_path[BUF_SIZE];
            snprintf(password_file_path, BUF_SIZE, "%s/mypassword", home_dir);
            int password_fd = open(password_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (password_fd == -1) {
                perror("Failed to open mypassword file");
                return 2;
            }
            ssize_t written_bytes = write(password_fd, buffer, read_bytes);
            if (written_bytes == -1) {
                perror("Failed to write to mypassword file");
                return 2;
            }
            close(password_fd);

            // Display output of second command with digits replaced by '*'
            replace_digits_with_asterisks(buffer);
            printf("%.*s", (int)read_bytes, buffer);

            // Close remaining pipe
            close(pipe2[0]); // Close read end of pipe 2

            // Wait for second command to finish
            int status;
            waitpid(pid2, &status, 0);
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Command 2 failed\n");
                return 2;
            }
        }
    }

    return 0;
}