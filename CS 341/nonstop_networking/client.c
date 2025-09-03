/**
 * nonstop_networking
 * CS 341 - Spring 2024
 */

// Mucho code-o was yoinked from charming_chatroom

#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// errno
#include <errno.h>

// fstat
#include <sys/stat.h>

// open
#include <fcntl.h>

#include "common.h"

#define PAGE_SIZE (4096)

// TODO: Implement a fixed sized buffer for processing files, limit file names to 255 bytes

static int serverSocket;
static verb method;

void str_to_binary(const char *message, size_t len);
void client_get(char *remote, char *local);
void client_put(char *remote, char *local);
void client_delete(char *remote);
void client_list();
char *process_ok_from_server();
void process_error_from_server();
char *process_response_from_server(char *response);
char *read_response_from_server();
void write_command_to_server(char *cmd, char *address);
int connect_to_server(const char *host, const char *port);
char **parse_args(int argc, char **argv);
verb check_args(char **args);

int main(int argc, char **argv) {
    // Good luck!
    char **parsed_args = parse_args(argc, argv);
    if (parsed_args == NULL) {
        print_client_usage();
        exit(1);
    }
    // Get the parsed args
    char *server = parsed_args[0];
    char *port = parsed_args[1];
    method = check_args(parsed_args);
    char *remote = parsed_args[3];
    char *local = parsed_args[4];

    // Connect to the server
    serverSocket = connect_to_server(server, port);

    switch(method) {
        case GET:
            client_get(remote, local);
            break;
        case PUT:
            client_put(remote, local);
            break;
        case DELETE:
            client_delete(remote);
            break;
        case LIST:
            client_list();
            break;
        default:
            // This should never happen, as check_args() should catch an invalid method
            print_client_help();
            exit(1);
    }
    free(parsed_args);
    parsed_args = NULL;
}

void client_get(char *remote, char *local) {
    write_command_to_server("GET ", remote);
    shutdown(serverSocket, SHUT_WR);
    char *response = read_response_from_server();
    if (response == NULL) {
        print_invalid_response();
        exit(1);
    }
    char *data = process_response_from_server(response);
    if (data == NULL) {
        print_invalid_response();
        exit(1);
    }
    if (strcmp(data, "") == 0) {
        print_connection_closed();
        exit(1);
    }
    size_t data_len = strlen(data);
    FILE *f = fopen(local, "w+");
    if (fwrite(data, 1, data_len, f) != data_len) {
        perror("client_get(): fwrite wrote inaccurate number of bytes\n");
        exit(1);
    }

    // Cleanup
    fclose(f);
    f = NULL;
    free(response);
    response = NULL;
    free(data);
    data = NULL;
}

void client_put(char *remote, char *local) {
    struct stat info;
    if (stat(local, &info) == -1) {
        print_client_help();
        exit(1);
    }

    write_command_to_server("PUT ", remote);
    size_t len = info.st_size;
    int retval = send_message_size(serverSocket, len, 0);
    if (retval == -1) {
        if (errno == EPIPE) {
            print_connection_closed();
            exit(1);
        }
        print_invalid_response();
        exit(1);
    }

    int fd = open(local, O_RDONLY);
    if (fd == -1) {
        print_client_help();
        exit(1);
    }

    retval = write_file_to_socket(serverSocket, fd, len, 0);
    if (retval == -1) {
        if (errno == EPIPE) {
            print_connection_closed();
            exit(1);
        }
        print_invalid_response();
        exit(1);
    }
    shutdown(serverSocket, SHUT_WR);

    char *response = read_response_from_server();
    if (response == NULL) {
        print_invalid_response();
        exit(1);
    }

    char *data = process_response_from_server(response);
    if (data == NULL) {
        print_invalid_response();
        exit(1);
    }
    if (strcmp(data, "") == 0) {
        print_connection_closed();
        exit(1);
    }
    print_success();

    // Cleanup
    close(fd);
    free(response);
    response = NULL;
}

void client_delete(char *remote) {
    write_command_to_server("DELETE ", remote);
    shutdown(serverSocket, SHUT_WR);
    char *response = read_response_from_server();
    if (response == NULL) {
        print_invalid_response();
        exit(1);
    }
    char *data = process_response_from_server(response);
    if (data == NULL) {
        print_invalid_response();
        exit(1);
    }
    if (strcmp(data, "") == 0) {
        print_connection_closed();
        exit(1);
    }
    print_success();

    // Cleanup
    free(response);
    response = NULL;
}

void client_list() {
    write_command_to_server("LIST", "");
    shutdown(serverSocket, SHUT_WR);
    char *response = read_response_from_server();
    if (response == NULL) {
        print_invalid_response();
        exit(1);
    }
    char *data = process_response_from_server(response);
    if (data == NULL) {
        print_invalid_response();
        exit(1);
    }
    if (strlen(data) > 0) {
        printf("%s\n", data);
        free(data);
        data = NULL;
    }
    // Cleanup
    free(response);
    response = NULL;
}

/**
 * Processes the response from the server. Returns "" if the connection is closed.
*/
char *process_ok_from_server() {
    // The error message is assumed to contain less than 4096 characters
    ssize_t expected_size = 0;
    ssize_t retval = get_file_size(serverSocket, (char *) &expected_size, 0);
    if (retval == -1) {
        if (errno == EPIPE) {
            return "";
        }
        print_invalid_response();
        exit(1);
    }
    char *buffer = calloc(1, expected_size + 1);
    int actual_size = read_all_from_socket(serverSocket, buffer, expected_size, 0);
    if (actual_size == -1) {
        if (errno == EPIPE) {
            return "";
        }
        print_invalid_response();
        exit(1);
    }
    if (actual_size < expected_size) {
        print_too_little_data();
        exit(1);
    }
    if (read(serverSocket, buffer + expected_size, 1) != 0) {
        print_received_too_much_data();
        exit(1);
    }
    return buffer;
}

/**
 * Processes the server's error message and exits the program.
 *
 * Returns the response allocated on the heap, "" if the socket is disconnected, or NULL on failure.
 * Throws an error on an invalid response.
 */
void process_error_from_server() {
    // The error message is assumed to contain less than 4096 characters
    char *buffer = calloc(1, PAGE_SIZE);
    size_t bytes_read = 0;
    while(bytes_read < PAGE_SIZE) {
        size_t cur_read = read(serverSocket, buffer + bytes_read, PAGE_SIZE - bytes_read);
        if (cur_read == 0) {
            break;
        }
        else if (cur_read > 0) {
            bytes_read += cur_read;
        }
        else if (cur_read < 0 && errno == EINTR) {
            continue;
        }
        else {
            break;
        }
    }
    // If the last character is a '\n' character, since print_error_message() already includes a newline, we don't need to include it.
    if (buffer[bytes_read - 1] == '\n') buffer[bytes_read - 1] = '\0';
    print_error_message(buffer);
    free(buffer);
    buffer = NULL;
    exit(1);
}

/**
 * Processes the server's response, and returns its followup message (if any)
 * 
 * Expected responses: "OK\n", "ERROR\n"
 *
 * Returns the response allocated on the heap, "" if the socket is disconnected, or NULL on failure.
 * Throws an error on an invalid response.
 */
char *process_response_from_server(char *response) {
    if (strcmp(response, "ERROR\n") == 0) {
        process_error_from_server();
    } else if (strcmp(response, "OK\n") == 0) {
        return process_ok_from_server();
    }
    print_invalid_response();
    exit(1);
}

/**
 * Attempts to read the response from the server to the client.
 * 
 * Expected responses: "OK\n", "ERROR\n"
 *
 * Returns the response allocated on the heap or NULL on failure.
 * Throws an error fon an invalid response.
 */
char *read_response_from_server() {
    // The maximum length that can be returned by the first line from the server is "ERROR\n"
    char *buffer = calloc(1, 7);
    size_t bytes_read = 0;
    while(bytes_read < 6) {
        size_t cur_read = read(serverSocket, buffer + bytes_read, 1);
        if (cur_read == 0) {
            return buffer;
        }
        else if (cur_read == 1) {
            bytes_read++;
        }
        else if (cur_read < 0 && errno == EINTR) {
            continue;
        }
        else {
            return NULL;
        }
        if (buffer[bytes_read - 1] == '\n') {
            break;
        }
    }
    if (strcmp(buffer, "OK\n") != 0 && strcmp(buffer, "ERROR\n") != 0) {
        print_invalid_response();
        exit(1);
    }
    return buffer;
}

/**
 * Reads the command and address from the user and writes them to the server.
 * 
 * cmd and address are combined as "%s%s\n"
 * 
 * cmd - char* A string spelling out the command, with an added space if the command isn't "LIST".
 * address - char* A string specifying the path to the desired file.
 */
void write_command_to_server(char *cmd, char *address) {
    if (strcmp(cmd, "LIST") == 0 && strlen(address) != 0) {
        perror("write_command_to_server(): Address is non-empty\n");
        exit(1);
    } 
    size_t cmd_len = strlen(cmd);
    size_t addr_len = strlen(address);
    // Allocate 1 extra character for the '\n', 1 extra character for '\0'
    char *msg = calloc(1, cmd_len + addr_len + 2);
    sprintf(msg, "%s%s\n", cmd, address);

    size_t len = strlen(msg);
    if (cmd_len + addr_len + 1 != len) {    // ???
        perror("write_command_to_server(): Invalid message len\n");
        exit(1);
    }

    ssize_t retval = write_all_to_socket(serverSocket, msg, len, 0);

    if (retval == -1) {
        if (errno == EPIPE) {
            print_connection_closed();
            exit(1);
        }
        perror("write_command_to_server(): Failed to write the message\n");
        exit(1);
    }
    
    // Cleanup
    free(msg);
    msg = NULL;
}

/**
 * Sets up a connection to a chatroom server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or exit(1) on failure.
 */
int connect_to_server(const char *host, const char *port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { // Error
        perror("connect_to_server(), socket(): ");
        exit(1);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *server_info;
    int s = getaddrinfo(host, port, &hints, &server_info);
    if (s != 0) {    // Error
        fprintf(stderr, "connect_to_server(), getaddrinfo(): %s\n", gai_strerror(s));
        exit(1);
    }

    s = connect(sockfd, server_info->ai_addr, server_info->ai_addrlen);
    if (s == -1) {    // Error
        perror("connect_to_server(), connect(): ");
        exit(1);
    }
    freeaddrinfo(server_info);
    server_info = NULL;

    return sockfd;
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}

void str_to_binary(const char *message, size_t len) {
    printf("%zu: ", len);
    size_t i;
    for (i = 0; i < len; i++) {
        printf("%d ", message[i]);
    }
    printf("\n");
}