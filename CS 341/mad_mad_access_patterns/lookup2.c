/**
 * mad_mad_access_patterns
 * CS 341 - Spring 2024
 */
#include "tree.h"
#include "utils.h"

#include <stdlib.h> // exit
#include <sys/mman.h> // mmap

#include <sys/stat.h> // open, stat
#include <fcntl.h>

#include <string.h> // strcmp
#include <stdbool.h>

static char *file_name;
static int file_size;

void *open_file(int argc, char **argv);
BinaryTreeNode* download_node(void *file, size_t offset);
void print_node(BinaryTreeNode *node);
BinaryTreeNode* search(FILE *file, char *word);

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/
int main(int argc, char **argv) {
    // Format checks
    void *file = open_file(argc, argv);
    // Search for each word
    int i;
    for (i = 2; i < argc; i++) {
        char *word = argv[i];
        BinaryTreeNode *node = search(file, word);
        if (node == NULL) {
            printNotFound(word);
        } else {
            printFound(node->word, node->count, node->price);
        }
    }
    if (munmap(file, file_size) == -1) {
        perror("munmap");
        exit(3);
    }
    file = NULL;
    return 0;
}

BinaryTreeNode* search(FILE *file, char *word) {
    BinaryTreeNode *cur_node = download_node(file, 4);  // The root node is stored at offset 4
    BinaryTreeNode *new_node = NULL;
    int diff = 0;
    // If strcmp < 0, then word is to the left of cur_node->word
    // If strcmp > 0, then word is to the right of cur_node->word
    while ((diff = strcmp(word, cur_node->word)) != 0) {
        if ((diff < 0 && cur_node->left_child == 0) || (diff > 0 && cur_node->right_child == 0)) {
            return NULL;
        }
        new_node = (diff < 0) ? download_node(file, cur_node->left_child) : download_node(file, cur_node->right_child);
        cur_node = new_node;
        new_node = NULL;
    }
    return cur_node;
}

void print_node(BinaryTreeNode *node) {
    printf("Left Offset: %u\n", node->left_child);
    printf("Right Offset: %u\n", node->right_child);
    printf("Count: %u\n", node->count);
    printf("Price: %f\n", node->price);
    printf("Word: %s\n", node->word);
}

/**
 * Returns a pointer to a BinaryTreeNode at the specified offset in the given file
*/
BinaryTreeNode* download_node(void *file, size_t offset) {
    // Set node to the address of file + offset, which should be the start of a BinaryTreeNode
    return (BinaryTreeNode*) (file + offset);
}

/**
 * Verifies the arguments are valid before returning a pointer to the opened file.
*/
void *open_file(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }
    file_name = argv[1];
    int f = open(file_name, O_RDONLY);
    if (f == -1) {
        openFail(file_name);
        exit(2);
    }
    struct stat st;
    if (stat(file_name, &st) == -1) {
        mmapFail(file_name);
        exit(2);
    }
    file_size = st.st_size;
    void *file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);
    if (file == MAP_FAILED) {
        mmapFail(file_name);
        exit(3);
    }
    
    // Check that the first 4 characters spell out "BTRE"
    char f_type[5];
    memset(f_type, 0, 5);
    if (memcpy(f_type, file, 4) == 0) {
        mmapFail(file_name);
        exit(2);
    }
    if (strcmp(f_type, "BTRE") != 0) {
        formatFail(file_name);
        exit(2);
    }
    return file;
}