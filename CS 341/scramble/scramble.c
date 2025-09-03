// author: ytlin4
// Compiling command: clang -o scramble -O0 -Wall -Wextra -Werror -g -std=c99 -D_GNU_SOURCE -DDEBUG scramble.c

#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>    // Bool

#include <sys/stat.h>   // stat
#include <fcntl.h>      // open, pipe

#include <unistd.h>     // access, dup2, pipe

#include <string.h>     // strings stuff

#include <sys/mman.h>   // mmap

#include <errno.h>      // errno

// # Not copyright

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

void validate_args(int argc) {
    // If 2 arguments are not specified, display a usage message on stderr and exit with a value of 1.
    if (argc != 3) {
        perror("./scramble <file> <file>\n");
        exit(1);
    }
}

int main(int argc, char **argv) {
    validate_args(argc);

    // If the files cannot be opened or accessed, print a message using perror and exit with a value of 2.
    struct stat info1, info2;
    if ((lstat(argv[1], &info1) != 0) || (lstat(argv[2], &info2) != 0)) {
        perror("File(s) don't exist");
        exit(2);
    } else if ((access(argv[1], R_OK | W_OK) != 0) || (access(argv[2], R_OK | W_OK) != 0)) {
        perror("File(s) can't be edited");
        exit(2);
    }
    // Otherwise, if either file is a symbolic link then print Symlink! and exit with a value of 3.
    if (S_ISLNK(info1.st_mode) || S_ISLNK(info2.st_mode)) {
        printf("Symlink!\n");
        exit(3);
    }

    // Otherwise, if either file is 0-length (i.e. empty) print Empty! and exit with a value of 4.
    if (info1.st_size == 0 || info2.st_size == 0) {
        printf("Empty!\n");
        exit(4);
    }
    long bytes_to_swap = min(info1.st_size, info2.st_size);

    // Uses mmap to map the file(s)’ contents into writeable memory.
    int fd1 = open(argv[1], O_RDWR);
    char *file1 = NULL;
    if ((file1 = mmap(NULL, bytes_to_swap, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0)) == (void *) -1) {
        perror("mmap failed!\n");
        exit(2);
    }
    int fd2 = open(argv[2], O_RDWR);
    char *file2 = NULL;
    if ((file2 = mmap(NULL, bytes_to_swap, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0)) == (void *) -1) {
        perror("mmap failed!\n");
        exit(2);
    }

    if (info1.st_ino != info2.st_ino) {
        // In normal mode, swap every other byte of the two given files using the memory-mapping.
        for (long i = 0; i < bytes_to_swap; i += 2) {
            char temp = file1[i];
            file1[i] = file2[i];
            file2[i] = temp;
        }
    } else {
        // Prints Reversing if the two files are the same inode on the same device.
        printf("Reversing\n");
        for (long i = 0; i < bytes_to_swap / 2; i++) {
            char temp = file1[i];
            file1[i] = file1[bytes_to_swap - i - 1];
            file1[bytes_to_swap - i - 1] = temp;
        }
    }
    // Exits normally with a value 0.
    return 0;    
}