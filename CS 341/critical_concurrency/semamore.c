/**
 * critical_concurrency
 * CS 341 - Spring 2024
 */
// Code reused from Fall 2023

#include <stdio.h>

#include <unistd.h>

#include "semamore.h"

/**
 * Initializes the Semamore. Important: the struct is assumed to have been
 * allocated by the user.
 * Example:
 * 	Semamore *s = malloc(sizeof(Semamore));
 * 	semm_init(s, 5, 10);
 *
 */
void semm_init(Semamore *s, int value, int max_val) {
    /* Your code here */
    if (s == NULL) {
        return;
    }
    
    s->value = value;
    s->max_val = max_val;

    s->m = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s->cv = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
}

/**
 *  Should block when the value in the Semamore struct (See semamore.h) is at 0.
 *  Otherwise, should decrement the value.
 */
void semm_wait(Semamore *s) {
    /* Your code here */
    if (s == NULL) {
        return;
    }

    if (s->value == 0) {
        pthread_mutex_lock(&(s->m));
        while(s->value == 0) {
            sched_yield();
        }
        s->value--;
        pthread_mutex_unlock(&(s->m));
    } else {
        s->value--;
    }
}

/**
 *  Should block when the value in the Semamore struct (See semamore.h) is at
 * max_value.
 *  Otherwise, should increment the value.
 */
void semm_post(Semamore *s) {
    /* Your code here */
    if (s == NULL) {
        return;
    }
    
    if (s->value == s->max_val) {
        pthread_mutex_lock(&(s->m));
        while(s->value == s->max_val) {
            sched_yield();
        }
        s->value++;
        pthread_mutex_unlock(&(s->m));
    } else {
        s->value++;
    }
}

/**
 * Takes a pointer to a Semamore struct to help cleanup some members of the
 * struct.
 * The actual Semamore struct must be freed by the user.
 */
void semm_destroy(Semamore *s) {
    /* Your code here */
    if (s == NULL) {
        return;
    }
    
    pthread_mutex_destroy(&(s->m));
    pthread_cond_destroy(&(s->cv));
}