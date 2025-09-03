/**
 * nonstop_networking
 * CS 341 - Spring 2024
 */
#include "common.h"

// errno
#include <errno.h>

// read
#include <unistd.h>

// calloc
#include <stdlib.h>

/**
 * Attempts to write all count bytes from buffer to socket.
 * Assumes buffer contains at least count bytes.
 *
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
 */
ssize_t write_all_to_socket(int socket, const char *buffer, size_t count, ssize_t offset) {
    size_t bytes_written = offset;
    while(bytes_written < count) {
        ssize_t cur_wrote = write(socket, buffer + bytes_written, count - bytes_written);
        if (cur_wrote < 0 && errno == EINTR) {
            continue;
        } else if (cur_wrote == 0 || cur_wrote == -1) {
            break;
        } else if (cur_wrote > 0) {
            bytes_written += cur_wrote;
        } else {
            return -1;
        }
    }
    return bytes_written;
}

/**
 * Attempts to read all count bytes from socket to buffer.
 * Assumes buffer contains at least count bytes.
 *
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
 */
ssize_t read_all_from_socket(int socket, char *buffer, size_t count, ssize_t offset) {
    size_t bytes_read = offset;
    while(bytes_read < count) {
        ssize_t cur_read = read(socket, buffer + bytes_read, count - bytes_read);
        if (cur_read < 0 && errno == EINTR) {
            continue;
        } else if (cur_read == 0 || cur_read == -1) {
            break;
        } else if (cur_read > 0) {
            bytes_read += cur_read;
        } else {
            return -1;
        }
    }
    return bytes_read;
}

void print_args(char **args) {
    size_t i;
    for (i = 0; args[i] != NULL; i++) {
        LOG("%s", args[i]);
    }
}

/**
 * Sends 'size' as a sequence of 8 character bytes to the given socket.
 * 
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
*/
ssize_t send_message_size(int socket, size_t size, int offset) {
    return write_all_to_socket(socket, (char *) &size, sizeof(size_t), offset);
}

/**
 * Write a file fd with 'size' bytes of data to the given socket.
 * 
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
*/
ssize_t write_file_to_socket(int socket, int fd, size_t size, int offset) {
    char *buffer = calloc(1, size + 1);
    int retval = read_all_from_socket(fd, buffer, size, offset);
    if (retval == -1) {
        return -1;
    }
    retval = write_all_to_socket(socket, buffer, size, offset);

    free(buffer);
    buffer = NULL;

    return retval;
}

/**
 * Gets the size of the file.
 * 
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
*/
ssize_t get_file_size(int socket, char *size, int offset) {
    return read_all_from_socket(socket, size, sizeof(ssize_t), offset);
}