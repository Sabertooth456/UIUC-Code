/**
 * charming_chatroom
 * CS 341 - Spring 2024
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Additional incudes
#include <unistd.h>
#include <errno.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    ssize_t hex_size = (ssize_t) htonl(size);
    ssize_t written_bytes = write_all_to_socket(socket, (char *)&hex_size, MESSAGE_SIZE_DIGITS);
    return written_bytes;
}

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

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
    ssize_t bytes_written = 0;
    while((size_t) bytes_written < count) {
        ssize_t cur_wrote = write(socket, buffer + bytes_written, count - bytes_written);
        if (cur_wrote == 0) {
            return bytes_written;
        }
        else if (cur_wrote > 0) {
            bytes_written += cur_wrote;
        }
        else if (cur_wrote == -1 && errno == EINTR) {
            continue;
        }
        else {
            return -1;
        }
    }
    return bytes_written;
}
