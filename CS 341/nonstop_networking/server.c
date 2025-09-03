/**
 * nonstop_networking
 * CS 341 - Spring 2024
 */
#include <stdio.h>

// mkdtemp
#include <stdlib.h>
// mkdir (testing purposes)
#include <sys/stat.h>
// open
#include <fcntl.h>

// memset
#include <string.h>

// rmdir, unlink
#include <unistd.h>

// opendir, readdir, closedir
#include <sys/types.h>
#include <dirent.h>

// errno
#include <errno.h>
#define NO_ERROR (0)

// bool
#include <stdbool.h>

// sigaction
#include <signal.h>

// EPOLL IS APPARENTLY NEEDED AAAHHAHAHHAHHAHAHA
#include <sys/epoll.h>

// Random includes from charming_chatroom
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include "format.h"
#include "common.h"

// TODO: Implement a fixed sized buffer, make code re-entrant

#define MAX_CLIENTS (16)

#define ARGS_MAX_LENGTH (7 + MP_FILENAME_MAX + 1 + MP_FILENAME_MAX + 2)

#define DATA_SIZE (sizeof(ssize_t))

static int serverSocket;
static int endSession;

static int clientsCount;
static int clients[MAX_CLIENTS];
static char *clientsHeader[MAX_CLIENTS];
static ssize_t clientsHeaderIndex[MAX_CLIENTS];    // If index == -1, then the header has been parsed
static char **clientsArgs[MAX_CLIENTS];
static char *clientsData[MAX_CLIENTS];
static ssize_t clientsReplyIndex[MAX_CLIENTS];      // If index == -1, then the reply has been written
static size_t clientsSize[MAX_CLIENTS];
static ssize_t clientsSizeIndex[MAX_CLIENTS];       // If index == -1, then the reply has been written
static ssize_t clientsDataIndex[MAX_CLIENTS];       // If index == -1, then the data has been parsed


static char *dir_path;

void create_directory();
void delete_directory();
bool is_file(struct dirent *dir);
static void signal_handler(int signum);
int run_server(char *port);
int process_client(int id);
void cleanup();
void print_args(char **args);
verb valid_command(char *buffer);
size_t free_args(char **args);
void write_error_message(int clientSocket, const char *error_message);

int main(int argc, char **argv) {
    // good luck!
    if (argc != 2) {
        print_server_usage();
        exit(1);
    }
    endSession = 0;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaddset(&(sa.sa_mask), SIGINT);
    sigaddset(&(sa.sa_mask), SIGPIPE);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("main, sigaction: ");
        exit(1);
    }
    create_directory();
    LOG("Storing files at %s", dir_path);
    LOG("Initializing server");
    int retval = run_server(argv[1]);
    cleanup();
    return retval;
}

char *get_rel_path(char *filename) {
    if (filename == NULL) {
        return NULL;
    }
    char *rel_path = calloc(1, 9 + MP_FILENAME_MAX + 1);    // "./" + "XXXXXX" + "/" + filename + "\0"
    rel_path[0] = '.';
    rel_path[1] = '/';
    for (size_t i = 0; i < 6; i++) {
        rel_path[i + 2] = dir_path[i];
    }
    if (strcmp(filename, "") == 0) {
        return rel_path;
    }
    rel_path[8] = '/';
    for (size_t i = 0; i < strlen(filename); i++) {
        rel_path[i + 9] = filename[i];
    }
    return rel_path;
}

/**
 * Cleanup function called in main after `run_server` exits.
 * Server ending clean up (such as shutting down clients) should be handled
 * here.
 */
void cleanup() {
    if (shutdown(serverSocket, SHUT_RDWR) != 0) {
        perror("cleanup, shutdown(serverSocket): ");
    }
    close(serverSocket);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            if (shutdown(clients[i], SHUT_RDWR) != 0) {
                perror("cleanup, shutdown(client): ");
            }
            close(clients[i]);
        }
    }

    delete_directory();
}

/**
 * Sets the value of the file descriptor fd to be blocking or nonblocking
 * For mode == true, sets the file descriptor to nonblocking
 * For mode == false, sets the file descriptor to blocking
 * 
 * Returns -1 on error
*/
int set_nonblocking(int fd, bool mode) {
    if (fd < 0) {
        perror("set_blocking: fd < 0\n");
        return -1;
    }
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("set_blocking, fcntl: ");
        return -1;
    }
    if (mode) {
        flags = flags | O_NONBLOCK;
    } else {
        flags = flags & ~O_NONBLOCK;
    }
    return (fcntl(fd, F_SETFL, flags) != -1);
}

void free_client(char **header, char ***args, char **data) {
    if (*header != NULL) {
        free(*header);
        *header = NULL;
    }
    if (*args != NULL) {
        free_args(*args);
        *args = NULL;
    }
    if (*data != NULL) {
        free(*data);
        *data = NULL;
    }
}

/**
 * Sets up a server connection.
 * Does not accept more than MAX_CLIENTS connections.  If more than MAX_CLIENTS
 * clients attempts to connects, simply shuts down
 * the new client and continues accepting.
 * Per client, a thread should be created and 'process_client' should handle
 * that client.
 * Makes use of 'endSession', 'clientsCount', 'client', and 'mutex'.
 *
 * port - port server will run on.
 *
 * If any networking call fails, the appropriate error is printed and the
 * function calls exit(1):
 *    - fprtinf to stderr for getaddrinfo
 *    - perror() for any other call
 * 
 * Returns 0 on success.
 * Returns -1 on error.
 */
int run_server(char *port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) { // Error
        perror("run_server, socket: ");
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    const int optval = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("run_server, setsockopt(SO_REUSEADDR): ");
        return 1;
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
        perror("run_server, setsockopt(SO_REUSEPORT): ");
        return 1;
    }
    
    struct addrinfo *server_info;
    int r_addr = getaddrinfo(NULL, port, &hints, &server_info);
    if (r_addr != 0) {    // Error
        fprintf(stderr, "run_server, getaddrinfo: %s\n", gai_strerror(r_addr));
        return 1;
    }

    if (bind(serverSocket, server_info->ai_addr, server_info->ai_addrlen) == -1) {    // Error check
        perror("run_server, bind: ");
        return 1;
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1) {    // Error check
        perror("run_server, listen: ");
        return 1;
    }

    freeaddrinfo(server_info);
    printf("Listening on port %s\n", port);

    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        perror("run_server, epollfd: ");
        return 1;
    }

    struct epoll_event event, events[MAX_CLIENTS];
    memset(&event, 0, sizeof(struct epoll_event));
    memset(&events, 0, sizeof(struct epoll_event) * MAX_CLIENTS);
    event.data.fd = serverSocket;
    event.events = EPOLLIN;

    int retval = epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &event);
    if (retval == -1) {
        perror("run_server, epoll_ctl: ");
        return 1;
    }
    
    // Initialize the client buffers
    for (size_t i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1;
        clientsHeader[i] = NULL;
        clientsHeaderIndex[i] = 0;
        clientsArgs[i] = NULL;
        clientsData[i] = NULL;
        clientsReplyIndex[i] = 0;
        clientsSize[i] = 0;
        clientsSizeIndex[i] = 0;
        clientsDataIndex[i] = 0;
    }

    int clientSocket = -1;
    struct sockaddr_storage client_addr;
    socklen_t client_size = sizeof(client_addr);
    while(endSession != 1) {
        clientsCount = epoll_wait(epollfd, events, MAX_CLIENTS, -1);
        if (clientsCount == -1) {
            perror("run_server, epoll_wait: ");
            return 1;    // TODO: Add error handling
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (events[i].data.fd == serverSocket) {
                clientSocket = accept(serverSocket, (struct sockaddr *) &client_addr, &client_size);
                if (clientSocket == -1) {
                    if (endSession == 1) {
                        break;
                    }
                    perror("run_server, accept: ");
                    exit(1);    // TODO: Add error handling
                }
                set_nonblocking(clientSocket, true);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientSocket;
                retval = epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSocket, &event);
                if (retval == -1) {
                    perror("run_server, epoll_ctl: ");
                    return 1;    // TODO: Add error handling
                }
                clients[i] = clientSocket;
                LOG("(fd=%i) Accepted new connection", clientSocket);
            } else if (clients[i] == -1) {
                continue;
            } else {
                clientSocket = clients[i];
                LOG("(fd=%i) Processing client", clientSocket);
                int retval = process_client(i);
                if (retval == 1) {
                    LOG("(fd=%i) Request successful", clientSocket);
                } else if (retval == -1) {
                    LOG("(fd=%i) Connection closed", clientSocket);
                } else if (retval == -2) {
                    write_error_message(clientSocket, err_no_such_file);
                    LOG("(fd=%i) No such file", clientSocket);
                } else if (retval == -3) {
                    write_error_message(clientSocket, err_bad_request);
                    LOG("(fd=%i) Server error", clientSocket);
                } else if (retval == -4 || retval == -6) {
                    write_error_message(clientSocket, err_bad_file_size);
                    LOG("(fd=%i) Bad file size", clientSocket);
                } else if (retval == -5) {
                    write_error_message(clientSocket, err_bad_request);
                    print_invalid_response();
                }
                if (retval != 0) {
                    // Close the connection
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, clientSocket, &event);
                    memset(&(events[i]), 0, sizeof(struct epoll_event));

                    if (shutdown(clientSocket, SHUT_RDWR) == -1) {
                        perror("run_server, shutdown: ");
                        exit(1);
                    }
                    close(clientSocket);

                    clients[i] = -1;
                    clientsHeaderIndex[i] = 0;
                    clientsReplyIndex[i] = 0;
                    clientsSize[i] = 0;
                    clientsSizeIndex[i] = 0;
                    clientsDataIndex[i] = 0;

                    free_client(&(clientsHeader[i]), &(clientsArgs[i]), &(clientsData[i]));
                }
            }
        }
    }
    return 0;
}

/**
 * Checks if the given name is equal to "GET ", "PUT ", "DELETE ", or "LIST\n".
 * Returns the matching substring as a verb, or V_UNKNOWN if it's invalid
*/
verb valid_command(char *buffer) {
    if (strcmp(buffer, "GET ") == 0) {
        return GET;
    } else if (strcmp(buffer, "PUT ") == 0) {
        return PUT;
    } else if (strcmp(buffer, "DELETE ") == 0) {
        return DELETE;
    } else if (strcmp(buffer, "LIST\n") == 0) {
        return LIST;
    }
    return V_UNKNOWN;
}

/**
 * Frees an array of character pointers. args must end in a NULL pointer.
 * 
 * Returns the amount of freed non-NULL pointers in args.
*/
size_t free_args(char **args) {
    if (args == NULL) {
        return 0;
    }
    size_t i = 0;
    while (args[i] != NULL) {
        free(args[i]);
        args[i] = NULL;
        i++;
    }
    free(args);
    return i;
}

/**
 * Reads the string of provided arguments and returns a array of character pointers to each string.
 * There can be at most 2 arguments, each argument separated by a space and ending in a newline.
 * 
 * Returns NULL when the arguments are improperly formatted.
*/
char **parse_args_from_client(char *buffer) {
    char **args = calloc(1, 3 * sizeof(char *));
    args[0] = calloc(1, 7 + 1);                 // Max size of command + '\0'
    args[1] = calloc(1, MP_FILENAME_MAX + 1);   // Max size of filename + '\0'
    bool valid_args = true;
    size_t len = strlen(buffer);
    size_t cur_arg = 0;
    size_t cur_i = 0;
    for (size_t total_i = 0; valid_args && total_i < len;) {
        if (cur_arg >= 2) {
            valid_args = false;
            break;
        } else if (buffer[total_i] == ' ') {
            cur_arg++;
            cur_i = 0;
            total_i++;
            continue;
        } else if (buffer[total_i] == '\n') {
            total_i++;
            break;
        }
        switch(cur_arg) {
            case (0):
                args[0][cur_i++] = buffer[total_i++];
                if (cur_i > 7) {
                    valid_args = false;
                }
                break;
            case (1):
                args[1][cur_i++] = buffer[total_i++];
                if (cur_i > MP_FILENAME_MAX) {
                    valid_args = false;
                }
                break;
            case (2):
                args[2][cur_i++] = buffer[total_i++];
                if (cur_i > MP_FILENAME_MAX) {
                    valid_args = false;
                }
                break;
            default:
                valid_args = false;
                break;
        }
    }
    // Free the uninitialized values
    for (size_t i = cur_arg + 1; i < 2; i++) {
        free(args[i]);
        args[i] = NULL;
    }
    if (!valid_args) {
        free_args(args);
        args = NULL;
    }
    return args;
}

// Returns a pointer if the buffer contains a newline, or NULL if it does not
char *has_newline(char *buffer) {
    return strchr(buffer, '\n');
}

/**
 * Reads in the header of the given clientSocket and fills in the given buffer, starting at the specified offset.
 * 
 * Returns -1 once a newline character has been read.
 * Returns -2 if the format is invalid.
 * Otherwise, returns the current length of the buffer.
*/
int read_header_from_client(int clientSocket, char *buffer, int offset) {
    if (buffer == NULL) {
        perror("read_header_from_client: buffer is NULL\n");
        exit(1);
    }
    if (has_newline(buffer) != NULL) {
        return -1;
    }
    size_t bytes_read = offset;
    while(bytes_read < ARGS_MAX_LENGTH) {
        size_t cur_read = read(clientSocket, buffer + bytes_read, 1);
        if (cur_read == 0) {
            break;
        }
        else if (cur_read == 1) {
            bytes_read++;
        }
        else if (cur_read < 0 && errno == EINTR) {
            continue;
        }
        else {
            perror("read_args_from_client, read: ");
            exit(1);
        }
        if (buffer[bytes_read - 1] == '\n') {
            return -1;
        }
        if (buffer[bytes_read - 1] == '\0') {
            return -2;
        }
    }
    if (bytes_read >= ARGS_MAX_LENGTH) {
        return -2;
    }
    return bytes_read;
}

/**
 * Returns the given string converted to a verb, or V_UNKNOWN for an invalid command.
*/
verb string_to_command(char *string) {
    if (strcmp(string, "GET") == 0) return GET;
    if (strcmp(string, "PUT") == 0) return PUT;
    if (strcmp(string, "DELETE") == 0) return DELETE;
    if (strcmp(string, "LIST") == 0) return LIST;
    return V_UNKNOWN;
}

/**
 * Loads the data of the specified filepath into clientsData and writes it back to the client.
 * 
 * Returns 1 on success.
 * Returns 0 when incomplete.
 * Returns -1 on a closed socket.
 * Returns -2 when the file doesn't exist.
 * Returns -3 on error.
 * Returns -4 on a data overflow.
 * Returns -5 on invalid format.
 * Returns -6 on a data underflow.
*/
int server_get(int id, char *filename) {
    char *filepath = get_rel_path(filename);
    if (filepath == NULL) {
        return -2;
    }
    int clientSocket = clients[id];

    // Check if the filepath is valid
    struct stat info;
    if (stat(filepath, &info) == -1) {
        free(filepath);
        filepath = NULL;
        return -2;
    }
    size_t len = info.st_size;

    // Check if the file can be opened
    LOG("(fd=%i) Opening the file", clientSocket);
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        free(filepath);
        filepath = NULL;
        return -2;
    }

    // Send the reply
    if (clientsReplyIndex[id] != -1) {
        int retval = write_all_to_socket(clientSocket, "OK\n", 3, clientsReplyIndex[id]);
        switch(retval) {
            case -1:
                close(fd);
                free(filepath);
                filepath = NULL;
                return -3;
            case 0:
                close(fd);
                free(filepath);
                filepath = NULL;
                if (errno == EPIPE) {
                    return -1;
                }
                return 0;
            case 3:
                clientsReplyIndex[id] = -1;
                LOG("(fd=%i) Finished writing response", clientSocket);
                break;
            default:
                clientsReplyIndex[id] = retval;
        }
    }
    
    // Send the message size
    if (clientsReplyIndex[id] == -1 && clientsSizeIndex[id] != -1) {
        ssize_t retval = send_message_size(clientSocket, len, clientsSizeIndex[id]);
        switch(retval) {
            case -1:
                close(fd);
                free(filepath);
                filepath = NULL;
                return -3;
            case 0:
                close(fd);
                free(filepath);
                filepath = NULL;
                if (errno == EPIPE) {
                    return -1;
                }
                return 0;
            case 8:
                clientsSizeIndex[id] = -1;
                LOG("(fd=%i) Sent message size", clientSocket);
                break;
            default:
                clientsSizeIndex[id] = retval;
        }
    }

    // Send the message
    if (clientsReplyIndex[id] == -1 && clientsSizeIndex[id] == -1 && clientsDataIndex[id] != -1) {
        ssize_t retval = write_file_to_socket(clientSocket, fd, len, clientsDataIndex[id]);
        if (retval == 0) {
            close(fd);
            free(filepath);
            filepath = NULL;
            if (errno == EPIPE) {
                return -1;
            }
            return 0;
        } else if (retval == -1) {
            close(fd);
            free(filepath);
            filepath = NULL;
            return -3;
        } else if ((size_t) retval == len) {
            clientsDataIndex[id] = -1;
            close(fd);
            free(filepath);
            filepath = NULL;
            LOG("(fd=%i) Message data sent", clientSocket);
            return 1;
        } else {
            clientsDataIndex[id] = retval;
        }
    }

    // Cleanup
    close(fd);
    free(filepath);
    filepath = NULL;

    return 0;
}

/**
 * Loads the data of the specified filepath into clientsData and writes it back to the client.
 * 
 * Returns 1 on success.
 * Returns 0 when incomplete.
 * Returns -1 on a closed socket.
 * Returns -2 when the file doesn't exist.
 * Returns -3 on error.
 * Returns -4 on a data overflow.
 * Returns -5 on invalid format.
 * Returns -6 on a data underflow.
*/
int server_put(int id, char *filename) {
    if (filename == NULL) {
        perror("server_put: filename is NULL\n");
    }
    // If a PUT request fails, delete the file.
    // If a PUT request is called with an existing file. overwrite the file.
    int clientSocket = clients[id];

    // Get the message size
    if (clientsSizeIndex[id] != -1) {
        ssize_t retval = get_file_size(clientSocket, (char *) (&(clientsSize[id])), clientsSizeIndex[id]);
        switch(retval) {
            case -1:
                return -3;
            case 0:
                if (errno == EPIPE) {
                    return -1;
                }
                return 0;
            case 8:
                clientsSizeIndex[id] = -1;
                LOG("(fd=%i) PUT requested for %s with file size %zu bytes", clientSocket, filename, (size_t) clientsSize[id]);
                break;
            default:
                clientsSizeIndex[id] = retval;
        }
    }
    
    // Initialize the message buffer
    if (clientsSizeIndex[id] == -1 && clientsData[id] == NULL) {
        clientsData[id] = calloc(1, clientsSize[id] + 1);
    }

    // Get the message data
    if (clientsSizeIndex[id] == -1 && clientsData[id] != NULL && clientsDataIndex[id] != -1) {
        LOG("(fd=%i) Reading binary data from request", clientSocket);

        ssize_t retval = read_all_from_socket(clientSocket, clientsData[id], clientsSize[id], clientsDataIndex[id]);
        if (retval == 0) {
            if (errno == EPIPE) {
                return -1;
            }
            if ((size_t) retval < clientsSize[id]) {
                LOG("(fd=%i) Sent too little data", clientSocket);
                return -6;
            }
        } else if (retval == -1) {
            return -3;
        } else if ((size_t) retval == clientsSize[id]) {
            clientsDataIndex[id] = -1;
            LOG("\"%s\"", clientsData[id]);
            LOG("Length: %zu", retval);
            // Check for an overflow
            char *test = calloc(1, 2);
            read(clientSocket, test, 1);
            if (errno != EPIPE && test[0] != '\0') {
                free(test);
                test = NULL;
                LOG("(fd=%i) Sent too much data", clientSocket);
                return -4;
            }
            free(test);
            test = NULL;
        } else {
            clientsDataIndex[id] = retval;
        }
    }

    if (clientsSizeIndex[id] == -1 && clientsData[id] != NULL && clientsDataIndex[id] == -1 && clientsReplyIndex[id] != -1) {
        // Send the reply
        if (clientsReplyIndex[id] != -1) {
            int retval = write_all_to_socket(clientSocket, "OK\n", 3, clientsReplyIndex[id]);
            switch(retval) {
                case -1:
                    return -3;
                case 0:
                    if (errno == EPIPE) {
                        return -1;
                    }
                    return 0;
                case 3:
                    clientsReplyIndex[id] = -1;
                    LOG("(fd=%i) Finished writing response", clientSocket);
                    break;
                default:
                    clientsReplyIndex[id] = retval;
            }
        }
    }

    // Store the message
    if (clientsSizeIndex[id] == -1 && clientsData[id] != NULL && clientsDataIndex[id] == -1 && clientsReplyIndex[id] == -1) {
        LOG("(fd=%i) Storing the message", clientSocket);
        char *filepath = get_rel_path(filename);
        int f = open(filepath, O_CREAT | O_TRUNC | O_WRONLY, 0777);

        if (f == -1) {
            close(f);
            free(filepath);
            filepath = NULL;
            perror("server_put, open: ");
            return 1;
        }

        ssize_t retval = write_all_to_socket(f, clientsData[id], clientsSize[id], 0);
        if (retval == -1) {
            // EPIPE is inpossible because the file descriptor is a local file
            close(f);
            free(filepath);
            filepath = NULL;
            perror("server_put, write_all_to_socket: ");
            return 1;
        }
        
        // Cleanup
        close(f);
        free(filepath);
        filepath = NULL;
        return 1;
    }
    return 0;
}

/**
 * Returns a list of files currently stored in the directory.
 * 
 * Returns 1 on success.
 * Returns 0 when incomplete.
 * Returns -1 on a closed socket.
 * Returns -2 when the file doesn't exist.
 * Returns -3 on error.
 * Returns -4 on a data overflow.
 * Returns -5 on invalid format.
 * Returns -6 on a data underflow.
*/
int server_list(int id) {
    int clientSocket = clients[id];
    
    // Initialize the buffer
    if (clientsData[id] == NULL) {
        char *rel_path = get_rel_path("");
        DIR *directory = opendir(rel_path);
        struct dirent *cur_dir = readdir(directory);
        size_t total_files = 0;
        while (cur_dir != NULL)
        {
            if (cur_dir->d_type == DT_REG) {
                total_files++;
            }
            cur_dir = readdir(directory); // Do not need to free this pointer
        }
        clientsData[id] = calloc(1, (MP_FILENAME_MAX + 1) * total_files + 1);
        LOG("(fd=%i) %zu files counted", clientSocket, total_files);

        closedir(directory);
        directory = opendir(rel_path);

        free(rel_path);
        rel_path = NULL;

        cur_dir = readdir(directory);
        size_t index = 0;
        while (cur_dir != NULL)
        {
            if (cur_dir->d_type == DT_REG) {
                total_files++;
                char *name = cur_dir->d_name;
                strcpy(clientsData[id] + index, name);
                index += strlen(name);
                clientsData[id][index] = '\n';
                index++;
            }
            cur_dir = readdir(directory); // Do not need to free this pointer
        }
        if (errno != NO_ERROR) {
            perror("server_list, readdir: ");
            return -3;
        }
        clientsData[id][index - 1] = '\0';
        LOG("(fd=%i) %zu files stored in buffer", clientSocket, total_files);

        free(rel_path);
        rel_path = NULL;
        closedir(directory);
        directory = NULL;
    }

    // Send the reply
    if (clientsData[id] != NULL && clientsReplyIndex[id] != -1) {
        int retval = write_all_to_socket(clientSocket, "OK\n", 3, clientsReplyIndex[id]);
        switch(retval) {
            case -1:
                return -3;
            case 0:
                if (errno == EPIPE) {
                    return -1;
                }
                return 0;
            case 3:
                clientsReplyIndex[id] = -1;
                LOG("(fd=%i) Finished writing response", clientSocket);
                break;
            default:
                clientsReplyIndex[id] = retval;
        }
    }
    
    // Send the message size
    if (clientsData[id] != NULL && clientsReplyIndex[id] == -1 && clientsSizeIndex[id] != -1) {
        size_t len = strlen(clientsData[id]);
        ssize_t retval = send_message_size(clientSocket, len, clientsSizeIndex[id]);
        switch(retval) {
            case -1:
                return -3;
            case 0:
                if (errno == EPIPE) {
                    return -1;
                }
                return 0;
            case 8:
                clientsSizeIndex[id] = -1;
                LOG("(fd=%i) Sent %zu as the message size", clientSocket, len);
                if (len == 0) {
                    return 1;
                }
                break;
            default:
                clientsSizeIndex[id] = retval;
        }
    }

    // Send the message
    if (clientsData[id] != NULL && clientsReplyIndex[id] == -1 && clientsSizeIndex[id] == -1) {
        size_t len = strlen(clientsData[id]);
        ssize_t retval = write_all_to_socket(clientSocket, clientsData[id], len, clientsDataIndex[id]);
        if (retval == 0) {
            if (errno == EPIPE) {
                return -1;
            }
            return 0;
        } else if (retval == -1) {
            return -3;
        } else if ((size_t) retval == len) {
            clientsDataIndex[id] = -1;
            LOG("(fd=%i) Message data sent", clientSocket);
            return 1;
        } else {
            clientsDataIndex[id] = retval;
        }
    }
    return 0;
}

/**
 * Returns a list of files currently stored in the directory.
 * 
 * Returns 1 on success.
 * Returns 0 when incomplete.
 * Returns -1 on a closed socket.
 * Returns -2 when the file doesn't exist.
 * Returns -3 on error.
 * Returns -4 on a data overflow.
 * Returns -5 on invalid format.
 * Returns -6 on a data underflow.
*/
int server_delete(int id, char *filename) {
    int clientSocket = clients[id];
    
    // Search through the directory to find the file
    if (clientsDataIndex[id] == 0) {
        char *filepath = get_rel_path(filename);
        if (unlink(filepath) == -1) {
            if (errno == ENOENT) {
                return -2;
            }
            perror("server_delete, unlink: ");
            return -3;
        }
        LOG("(fd=%i) Removed file with name %s", clientSocket, filename);
        clientsDataIndex[id] = -1;
    }

    // Send the reply
    if (clientsDataIndex[id] == -1) {
        int retval = write_all_to_socket(clientSocket, "OK\n", 3, clientsReplyIndex[id]);
        switch(retval) {
            case -1:
                return -3;
            case 0:
                if (errno == EPIPE) {
                    return -1;
                }
                return 0;
            case 3:
                clientsReplyIndex[id] = -1;
                LOG("(fd=%i) Finished writing response", clientSocket);
                return 1;
            default:
                clientsReplyIndex[id] = retval;
        }
    }
    return 0;
}


bool valid_args(char **args) {
    if (args == NULL) {
        return false;
    }
    if (args[0] == NULL && strlen(args[0]) > 7) {
        return false;
    }
    if (args[1] != NULL && strlen(args[1]) > MP_FILENAME_MAX) {
        return false;
    }
    return true;
}

void write_error_message(int clientSocket, const char *error_message) {
    write_all_to_socket(clientSocket, "ERROR\n", 6, 0);
    write_all_to_socket(clientSocket, error_message, strlen(error_message), 0);
}

/**
 * Handles the reading to and writing from clients.
 * id specifies the index of 'clients' that will be used.
 * 
 * Returns 1 on success.
 * Returns 0 when incomplete.
 * Returns -1 on a closed socket.
 * Returns -2 when the file doesn't exist.
 * Returns -3 on error.
 * Returns -4 on a data overflow.
 * Returns -5 on invalid format.
 * Returns -6 on a data underflow.
 */
int process_client(int id) {
    int clientSocket = clients[id];
    // Create the header buffer
    if (clientsHeader[id] == NULL) {
        clientsHeader[id] = calloc(1, ARGS_MAX_LENGTH);    // Command + Filename + ' ' + Filename + '\n' + '\0'
    }

    // Get the header data
    if (clientsHeader[id] != NULL && clientsHeaderIndex[id] != -1) {
        clientsHeaderIndex[id] = read_header_from_client(clientSocket, clientsHeader[id], clientsHeaderIndex[id]);
        if (clientsHeaderIndex[id] > 0) {
            return 0;
        }
        if (clientsHeaderIndex[id] == -2) {
            return -5;
        }
    }

    // Parse the header data
    if (clientsHeader[id] != NULL && clientsHeaderIndex[id] == -1 && clientsArgs[id] == NULL) {
        clientsArgs[id] = parse_args_from_client(clientsHeader[id]);
        if (clientsArgs[id] == NULL) {
            return -5;
        }
    }

    // Call the server command
    if (clientsHeader[id] != NULL && clientsHeaderIndex[id] == -1 && clientsArgs[id] != NULL) {
        if (!valid_args(clientsArgs[id])) {
            return -5;
        }
        verb cmd = string_to_command(clientsArgs[id][0]);

        switch (cmd) {
            case GET:
                LOG("(fd=%i) Running GET", clientSocket);
                return server_get(id, clientsArgs[id][1]);
            case PUT:
                LOG("(fd=%i) Running PUT", clientSocket);
                return server_put(id, clientsArgs[id][1]);
            case DELETE:
                LOG("(fd=%i) Running DELETE", clientSocket);
                return server_delete(id, clientsArgs[id][1]);
            case LIST:
                LOG("(fd=%i) Running LIST", clientSocket);
                return server_list(id);
            default:
                return -5;
        }
    }
    return 0;
}

/**
 * Catches certain signals and responds to them appropriately
*/
static void signal_handler(int signum) {
    if ((signum & SIGINT) != 0) {
        endSession = 1;
        cleanup();
        exit(0);
    }
    // SIGPIPE is ignored
}

/**
 * Initializes the server's directory with a random name of "XXXXXX", saves the path to 'dir_path', and opens it in 'directory'.
*/
void create_directory() {
    dir_path = calloc(1, 7);
    memset(dir_path, 'X', 6);
    char *retval = mkdtemp(dir_path);
    // dir_path = "temp_dir";   // Code for testing purposes
    // int retval = mkdir(dir_path, 0x0700);
    if (retval == NULL) {
        perror("initialize_directory, mkdir: ");
        exit(1);
    }
    print_temp_directory(dir_path);
}

// Checks if the given directory entry is a file
bool is_file(struct dirent *dir) {
    return (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, ".." ) != 0);
}
/**
 * Deletes all files stored in the server's directory before removing the directory itself. 
*/
void delete_directory() {
    char *rel_path = get_rel_path("");
    DIR *directory = opendir(rel_path);
    // Cleanup
    free(rel_path);
    rel_path = NULL;

    struct dirent *cur_dir = readdir(directory);
    while (cur_dir != NULL)
    {
        char *name = cur_dir->d_name;
        char *rel_name = get_rel_path(name);
        if (cur_dir->d_type == DT_REG && unlink(rel_name) == -1) {
            perror("delete_directory, unlink: ");
            exit(1);
        }
        free(rel_name);
        rel_name = NULL;
        cur_dir = readdir(directory); // Do not need to free this pointer
    }
    if (errno != NO_ERROR) {
        perror("delete_directory, readdir: ");
        exit(1);
    }

    if (closedir(directory) == -1) {
        perror("delete_directory, closedir: ");
        exit(1);
    }
    directory = NULL;

    if (rmdir(dir_path) == -1) {
        perror("delete_directory, rmdir: ");
        exit(1);
    }
    dir_path = NULL;
}