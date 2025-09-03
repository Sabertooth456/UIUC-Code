/**
 * mad_mad_access_patterns
 * CS 341 - Spring 2024
 */
#include "tree.h"
#include "utils.h"

#include <stdlib.h> // exit
#include <stdio.h>  // fopen, fread, fseek
#include <string.h> // strcmp
#include <stdbool.h>

#define MAX_WORD_SIZE 256

static char *file_name;

FILE *open_file(int argc, char **argv);
BinaryTreeNode* download_node(FILE *file, size_t offset);
void print_node(BinaryTreeNode *node);
BinaryTreeNode* search(FILE *file, char *word);

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/
int main(int argc, char **argv) {
    // Format checks
    FILE *file = open_file(argc, argv);
    // Search for each word
    int i;
    for (i = 2; i < argc; i++) {
        char *word = argv[i];
        BinaryTreeNode *node = search(file, word);
        if (node == NULL) {
            printNotFound(word);
        } else {
            printFound(node->word, node->count, node->price);
            // Free the node
            free(node);
            node = NULL;
        }
    }
    fclose(file);
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
            free(cur_node);
            cur_node = NULL;
            return NULL;
        }
        new_node = (diff < 0) ? download_node(file, cur_node->left_child) : download_node(file, cur_node->right_child);
        free(cur_node);
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
 * Returns a pointer to a BinaryTreeNode stored on the heap.
 * The returned node must be freed.
*/
BinaryTreeNode* download_node(FILE *file, size_t offset) {
    int err = fseek(file, offset, SEEK_SET);
    if (err == -1) {
        openFail(file_name);
        exit(2);
    }
    // Allocate room for the non-variable values as well as a MAX_WORD_SIZE byte ofset for the word (including the NULL pointer)
    BinaryTreeNode *node = calloc(1, sizeof(BinaryTreeNode) + MAX_WORD_SIZE + 1);
    // Download the non-variable values of the BinaryTreeNode.
    if (fread(node, sizeof(BinaryTreeNode), 1, file) == 0 && ferror(file)) {
        openFail(file_name);
        exit(2);
    }

    // If the word exceeds 255 characters, cut it early with a null character
    char *buffer = node->word;
    int len = 0;
    // Continue reading the word, one character at a time, until the null character is reached.
    while(fread(buffer + len, 1, 1, file) != 0 && buffer[len++] != '\0' && len < MAX_WORD_SIZE);
    return node;
}

/**
 * Verifies the arguments are valid before returning a pointer to the opened file.
*/
FILE *open_file(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }
    file_name = argv[1];
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        openFail(file_name);
        exit(2);
    }
    
    // Check that the first 4 characters spell out "BTRE"
    char f_type[5];
    memset(f_type, 0, 5);
    if (fread(f_type, 4, 1, file) == 0) {
        openFail(file_name);
        exit(2);
    }
    if (strcmp(f_type, "BTRE") != 0) {
        formatFail(file_name);
        exit(2);
    }
    return file;
}