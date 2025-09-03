/**
 * extreme_edge_cases
 * CS 341 - Spring 2024
 */
#include "camelCaser.h"
#include <stdlib.h>

#include <stdio.h>  // printf(), perror()
#include <ctype.h>  // ispunct(), isspace(), isalpha()
#include <string.h>
#include <stdbool.h>

size_t get_sentence_length(const char *input_str, size_t start_index) {
    size_t input_len = strlen(input_str);
    size_t length = 0;
    size_t i = start_index;
    for (; i < input_len; i++) {
        length++;
        char c = input_str[i];
        if (ispunct(c)) {
            return length - 1;
        }
    }
    return length;
}

// // Returns if the sentence starting from start_index only has normal characters
// bool has_normal_characters(const char *input_str, size_t start_index) {
//     size_t i = 0;
//     size_t sentence_len = get_sentence_length(input_str, start_index);
//     for (; i < sentence_len; i++) {
//         char c = input_str[start_index + i];
//         if (!ispunct(c) && !isspace(c) && !isalpha(c) && !__isascii(c)) {
//             return false;
//         }
//     }
//     return true;
// }

char **camel_caser(const char *input_str) {
    // TODO: Implement me!
    
    // Return NULL if input is NULL
    if (input_str == NULL) {
        return NULL;
    }
    // Get the length of the input
    size_t input_len = strlen(input_str);
    
    // Count the total number of sentences
    size_t total_sentences = 0;
    size_t i;
    for (i = 0; i < input_len; i++) {
        if (ispunct(input_str[i])) {
            total_sentences++;
        }
    }    
    // Allocate an array with pointers equal to the number of total sentences
    char **output_s = calloc(total_sentences + 1, sizeof(char *));
    size_t cur_index = 0;
    for (i = 0; i < total_sentences; i++) {
        // Allocate the pointers with a size equal to the length of the next sentence, excluding the punctuation mark
        size_t sentence_len = get_sentence_length(input_str, cur_index);
        output_s[i] = calloc(sentence_len + 1, 1);

        // bool normal_sentence = has_normal_characters(input_str, cur_index);
        // printf("Normal sentence: %i\n", normal_sentence);
        // printf("Sentence Length: %zu\n", sentence_len);
        bool prev_space = false;
        bool first_word = true;
        size_t sentence_j = 0;
        size_t j;
        for (j = 0; j < sentence_len; j++) {
            char c = input_str[cur_index + j];
            // if (normal_sentence) {
                if (isalpha(c)) {
                    if (prev_space && !first_word) {
                        c = toupper(c);
                        
                    } else {
                        c = tolower(c);
                    }
                    output_s[i][sentence_j] = c;
                    sentence_j++;
                    prev_space = false;
                    first_word = false;
                }
                else if (isspace(c)) {
                    prev_space = true;
                }
                else if (ispunct(c)) {
                    perror("camel_caser(): Character has a punctuation mark.\n");
                    exit(1);
                } else {
                    output_s[i][sentence_j] = c;
                    sentence_j++;
                    first_word = false;
                }
            // } else {
            //     output_s[i][sentence_j] = c;
            //     sentence_j++;
            // }
        }
        cur_index += sentence_len + 1;
    }
    // output_s[total_sentences] = NULL;
    return output_s;
}

void destroy(char **result) {
    // TODO: Implement me!
    int i = 0;
    while(result[i] != NULL) {
        free(result[i]);
        i++;
    }
    free(result);
    return;
}
