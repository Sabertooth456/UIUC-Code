/**
 * deepfried_dd
 * CS 341 - Spring 2024
 */
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Stuff

#include <time.h> // clock_gettime()

#include <fcntl.h> // fopen, fread, fwrite

#include <signal.h>

#include <stdbool.h> // Lol

#include <string.h> // Strings

static volatile bool continue_transfer = true;

void signal_handler(int dummy) {
    continue_transfer = false;
}

int main(int argc, char **argv) {
    // Signal handler for SIGUSR1
    signal(SIGUSR1, signal_handler);

    /**
     * -i <file>: input file (defaults to stdin)
     * -o <file>: output file (defaults to stdout)
     *      You should create this file if does not already exist.
     * -b <size>: block size, the number of bytes copied at a time (defaults to 512)
     * -c <count>: total number of blocks copied (defaults to the entire file)
     * -p <count>: number of blocks to skip at the start of the input file (defaults to 0)
     * -k <count>: number of blocks to skip at the start of the output file (defaults to 0)
    */
    struct timespec start, finish;
    clock_gettime(CLOCK_REALTIME, &start);
    
    int opt;
    char *f_i = NULL;   // NULL == stdin
    char *f_o = NULL;   // NULL == stdout
    size_t f_b = 512;   // Default is 512 bytes per block
    size_t f_c = -1;    // Default is the entire file
    size_t f_p = 0;     // Defaults to 0
    size_t f_k = 0;     // Defaults to 0
    while((opt = getopt(argc, argv, "i:o:b:c:p:k:")) != -1) {
        switch(opt) {
            case 'i':
                f_i = optarg;
                break;
            case 'o':
                f_o = optarg;
                break;
            case 'b':
                sscanf(optarg, "%zu", &f_b);
                break;
            case 'c':
                sscanf(optarg, "%zu", &f_c);
                break;
            case 'p':
                sscanf(optarg, "%zu", &f_p);
                break;
            case 'k':
                sscanf(optarg, "%zu", &f_k);
                break;
            default:
                perror("You shouldn't be here.\n");
                exit(1);
        }
    }

    // printf("Args: %s, %s, %i, %i, %i, %i\n", f_i, f_o, f_b, f_c, f_p, f_k);
    FILE *input_file = (f_i == NULL) ? stdin : fopen(f_i, "a+");
    if (input_file == NULL) {
        // Failed to open the file
        print_invalid_input(f_i);
        exit(1);
    }
    FILE *output_file;
    if (f_o == NULL) {
        output_file = stdout;
    } else if (access(f_o, F_OK) != 0) {
        output_file = fopen(f_o, "w+");
    } else {
        output_file = fopen(f_o, "r+");
    }
    if (output_file == NULL) {
        // Failed to open the file
        print_invalid_output(f_o);
        exit(1);
    }
    char *buffer = malloc(f_b + 1); // Enough to store the necessary blocks of memory, while also including a null byte.
    memset(buffer, 0, f_b + 1);

    /**
     * -p <count>: number of blocks to skip at the start of the input file (defaults to 0)
     * -k <count>: number of blocks to skip at the start of the output file (defaults to 0)
    */
    // Apply the -p and -k offsets
    // This is fine because fseek won't be tested for non-file IO streams
    if (f_p != 0) fseek(input_file, f_p, SEEK_SET);
    if (f_k != 0) fseek(output_file, f_k, SEEK_SET);

    size_t full_blocks_in = 0;
    size_t partial_blocks_in = 0;
    size_t full_blocks_out = 0;
    size_t partial_blocks_out = 0;
    size_t total_bytes_copied = 0;
    size_t r = 0;
    while(continue_transfer && (f_c == (size_t) -1 || full_blocks_in < f_c) && (feof(input_file) == 0) &&((r = fread(buffer, 1, f_b, input_file)) > 0)) {
        // printf("Buffer Length: %zu\n", r);
        // printf("Buffer: %s\n", buffer);
        if (r == f_b) {
            full_blocks_in++;
        } else {
            partial_blocks_in++;
        }

        size_t w = fwrite(buffer, 1, r, output_file); // Errors are undefined
        total_bytes_copied += w;
        if (w == f_b) {
            full_blocks_out++;
        } else {
            partial_blocks_out++;
        }
        memset(buffer, 0, f_b + 1);
    }
    clock_gettime(CLOCK_REALTIME, &finish);

    struct timespec change;
    change.tv_sec = finish.tv_sec - start.tv_sec;
    change.tv_nsec = finish.tv_nsec - start.tv_nsec;

    print_status_report(full_blocks_in, partial_blocks_in, full_blocks_out, partial_blocks_in, total_bytes_copied, (double)change.tv_sec + (double)change.tv_nsec / 1e9);

    // Cleanup
    free(buffer);
    buffer = NULL;
    fclose(input_file);
    input_file = NULL;
    fclose(output_file);
    output_file = NULL;
    return 0;
}