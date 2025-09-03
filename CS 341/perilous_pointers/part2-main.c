/**
 * perilous_pointers
 * CS 341 - Spring 2024
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Resubmition of code from Fall 2023

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
    
    // [1]
    first_step(81);
    
    // [2]
    int* i = malloc(sizeof(int));
    *i = 132;
    second_step(i);
    free(i);
    i = NULL;

    // [3]
    int** j = malloc(sizeof(int*));
    j[0] = malloc(sizeof(int));
    *j[0] = 8942;
    double_step(j);
    free(j[0]);
    j[0] = NULL;
    free(j);
    j = NULL;

    // [4]
    i = malloc(sizeof(int));
    *i = 15;
    strange_step((char *)i - 5);
    free(i);
    i = NULL;
    
    // [5]
    void *value = calloc(4, sizeof(char));
    empty_step(value);
    free(value);
    value = NULL;

    // [6]
    void *k = malloc(strlen("uuuu")+1);
    char *l = k;
    strcpy(l, "uuuu");
    two_step(k, l);
    free(k);
    k = NULL;
    l = NULL;

    // [7]
    char m[6] = "01234";
    three_step(&m[0], &m[2], &m[4]);
    
    // [8]
    char n[3] = "aa";
    char o[4] = "iii";
    char p[5] = "qqqq";
    step_step_step(n, o, p);

    // [9]
    char q = 'a';
    it_may_be_odd(&q, (int)q);

    // [10]
    char r[] = "CS241,CS241,CS241";
    tok_step(r);

    // [11]
    void* s = calloc(1, sizeof(int));
    int* t = (int *)s;
    char* u = (char *)s;
    *t = 0;
    u[0] = 1;
    *t += 512;
    // printf("u[0]:   %d\n", ((char *)s)[0]);
    // printf("*t: %d\n", (*((int *)s) % 3));
    // printf("t == u: %d\n", ((void *)t == (void *)u));
    the_end(s, s);

    free(s);
    s = NULL;
    t = NULL;
    u = NULL;

    return 0;
}