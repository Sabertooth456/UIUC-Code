/**
 * extreme_edge_cases
 * CS 341 - Spring 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

// Incomplete Tests: "a  b.", "a..b  .", "a. .b"

void print_camelCaser(char *input, char **result) {
    printf("Input:\n");
    printf("%s\n\n", input);
    printf("Output: [\n");
    int index = 0;
    char *cur_sentence = result[index];
    while (cur_sentence != NULL) {
        printf("\"%s\",\n", cur_sentence);
        cur_sentence = result[++index];
    }
    printf("NULL\n");
    printf("]\n\n");
}

int check_output(char **user_output, char **correct_output) {
    int i = 0;
    while(user_output[i] != NULL && correct_output[i] != NULL) {
        if (strcmp(user_output[i], correct_output[i]) != 0) {
            return 0;
        }
        i++;
    }
    // If both the user_output and correct_output end in NULL, then the user_output is correct
    return (correct_output[i] == user_output[i]);
}

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Implement me!
    char *input = "hello. welcome to cs241";
    char **result = camelCaser(input);
    // print_camelCaser(input, result);
    char *correct1[] = {"hello", NULL};
    if(!check_output(result, correct1)) {
        return 0;
    }
    destroy(result);
    result = NULL;
    // printf("Function is correct.\n\n");

    input = "The Heisenbug is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion.";
    result = camelCaser(input);
    // print_camelCaser(input, result);
    char *correct2[] = {"theHeisenbugIsAnIncredibleCreature",
    "facenovelServersGetTheirPowerFromItsIndeterminism",
    "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener",
    "godObjectsAreTheNewReligion",
    NULL};
    if(!check_output(result, correct2)) {
        return 0;
    }
    destroy(result);
    result = NULL;
    // printf("Function is correct.\n\n");

    input = "The Heisenbug▄ is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion";
    result = camelCaser(input);
    // print_camelCaser(input, result);
    char *correct3[] = {"theHeisenbug▄IsAnIncredibleCreature",
    "facenovelServersGetTheirPowerFromItsIndeterminism",
    "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener",
    NULL};
    if(!check_output(result, correct3)) {
        return 0;
    }
    destroy(result);
    result = NULL;

    input = "";
    result = camelCaser(input);
    // print_camelCaser(input, result);
    char *correct4[] = {NULL};
    if(!check_output(result, correct4)) {
        return 0;
    }
    destroy(result);
    result = NULL;

    // Null case
    input = NULL;
    result = camelCaser(input);
    if(result) {
        return 0;
    }

    input = "The Heisenbug▄ ▄s an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion";
    result = camelCaser(input);
    // print_camelCaser(input, result);
    char *correct5[] = {"theHeisenbug▄▄SAnIncredibleCreature",
    "facenovelServersGetTheirPowerFromItsIndeterminism",
    "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener",
    NULL};
    if(!check_output(result, correct5)) {
        return 0;
    }
    destroy(result);
    result = NULL;

    return 1;
}
