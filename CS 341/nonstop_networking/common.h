/**
 * nonstop_networking
 * CS 341 - Spring 2024
 */
#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <stdio.h>

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

#define MP_FILENAME_MAX (255)

/**
 * Attempts to write all count bytes from buffer to socket.
 * Assumes buffer contains at least count bytes.
 *
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
 */
ssize_t write_all_to_socket(int socket, const char *buffer, size_t count, ssize_t offset);

/**
 * Attempts to read all count bytes from socket to buffer.
 * Assumes buffer contains at least count bytes.
 *
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
 */
ssize_t read_all_from_socket(int socket, char *buffer, size_t count, ssize_t offset);

// Print the output of the parsed arguments, stopping at a NULL pointer
void print_args(char **args);

/**
 * Sends 'size' as a sequence of 8 character bytes to the given socket.
 * 
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
*/
ssize_t send_message_size(int socket, size_t size, int offset);

/**
 * Write a file fd with 'size' bytes of data to the given socket.
 * 
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
*/
ssize_t write_file_to_socket(int socket, int fd, size_t size, int offset);

/**
 * Gets the size of the file.
 * 
 * Returns 0 on a closed socket.
 * Returns -1 on an error.
 * Otherwise, returns the number of bytes written.
*/
ssize_t get_file_size(int socket, char *size, int offset);