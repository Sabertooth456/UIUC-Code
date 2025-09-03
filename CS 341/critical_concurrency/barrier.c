/**
 * critical_concurrency
 * CS 341 - Spring 2024
 */
// Code reused from Fall 2023

#include "barrier.h"

// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    int error = 0;
    pthread_mutex_destroy(&(barrier->mtx));
    pthread_cond_destroy(&(barrier->cv));
    
    return error;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    int error = 0;
    if (barrier == NULL) {
        return 0;
    }

    if (num_threads == 0) {
        return 1;
    }

    barrier->mtx = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    barrier->cv = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    barrier->n_threads = num_threads;
    barrier->count = 1;
    barrier->times_used = 0;

    return error;
}

int barrier_wait(barrier_t *barrier) {
    if (barrier == NULL) {
        return 0;
    }

    pthread_mutex_lock(&barrier->mtx);
    barrier->times_used++;
    if (barrier->times_used == barrier->n_threads) {
        barrier->times_used = 0;
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cv);
    } else {
        while(barrier->times_used != 0) {
            pthread_cond_wait(&barrier->cv, &barrier->mtx);
        }
    }

    // I made a second barrier to stop ambitious threads from the first barrier
    // Is this the right way to do it? I don't know lmao.
    barrier->count++;
    if (barrier->count != barrier->n_threads) {
        while(barrier->count != barrier->n_threads) {
            pthread_cond_wait(&barrier->cv, &barrier->mtx);
        }
    } else {
        pthread_cond_broadcast(&barrier->cv);
    }

    pthread_mutex_unlock(&barrier->mtx);
    return 0;
}