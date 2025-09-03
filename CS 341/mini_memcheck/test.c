/**
 * mini_memcheck
 * CS 341 - Spring 2024
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * Frees a bad pointer
 * Requested: 5, Freed: 5, Bad Calls: 1
*/
void test_1() {
    void *ptr = malloc(5);
    free(ptr);
    free(ptr);
}

/**
 * Reallocates a bad pointer
 * Requested: 6, Freed: 6, Bad Calls: 1
*/
void test_2() {
    void *ptr = malloc(6);
    free(ptr);
    ptr = realloc(ptr, 10);
}

/**
 * Tests calloc with a program that has no memory leaks
 * Requested: 32, Freed: 32, Bad Calls: 0
*/
void test_3() {
    size_t n = 4;
    size_t *ptr = calloc(n, sizeof(size_t));
    // size_t i;
    // for (i = 0; i < n; i++) {
    //     printf("ptr[%zu] = %p\n", i, &(ptr[i]));
    // }
    free(ptr); ptr = NULL;
}

/**
 * Tests with a program that has memory leaks
 * Requested: 63, Freed: 15, Bad Calls: 0
*/
void test_4() {
    void *ptr2 = malloc(15);
    size_t n = 6;
    size_t *ptr = calloc(n, sizeof(size_t));
    // size_t i;
    // for (i = 0; i < n; i++) {
    //     printf("ptr[%zu] = %p\n", i, &(ptr[i]));
    // }
    ptr[0] = 4;
    free(ptr2); ptr2 = NULL;
}

/**
 * Tests with a program that has no memory leaks
 * Requested: 64, Freed: 64, Bad Calls: 0
*/
void test_5() {
    size_t *ptr = malloc(sizeof(size_t));
    // printf("Ptr: %p\n", ptr);
    *ptr = 20;
    free(ptr); ptr = NULL;
    // printf("Ptr: %p\n", ptr);
    ptr = calloc(3, sizeof(size_t));
    ptr[0] = 0;
    ptr[1] = 1;
    ptr[3] = 2;
    // printf("Ptr: %p\n", ptr);
    ptr = realloc(ptr, 16);
    ptr = realloc(ptr, 16);
    ptr[0] = 3;
    ptr[1] = 4;
    ptr[3] = 5;
    ptr[4] = 6;
    // printf("Ptr: %p\n", ptr);
    free(ptr); ptr = NULL;
}

/**
 * Test realloc with a program that has no memory leaks
 * Requested: 40, Freed: 40, Bad Calls: 0
*/
void test_6() {
    size_t *ptr = malloc(sizeof(size_t));
    *ptr = 20;
    size_t n = 4;
    ptr = realloc(ptr, n * sizeof(size_t));
    size_t i;
    for (i = 0; i < n; i++) {
        ptr[i] = 16 + i;
    }
    free(ptr);
    ptr = NULL;
}

int main(int argc, char *argv[]) {
    // Your tests here using malloc and free
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    return 0;
}