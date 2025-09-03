/**
 * teaching_threads
 * CS 341 - Spring 2024
 */

// Code reused from Fall 2023

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <math.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
typedef struct Data {
    int *list;
    size_t list_len;
    reducer reduce_func;
    int base_case;
} data_t;

/* You should create a start routine for your threads. */

// // Might be unnecessary
// static pthread_mutex_t n;

// int reduce_safe(int *list, size_t length, reducer reduce_func, int base_case) {
//     int result = base_case;

//     for (size_t i = 0; i < length; ++i) {
//         pthread_mutex_lock(&n);
//         result = reduce_func(result, list[i]);
//         pthread_mutex_unlock(&n);
//     }

//     return result;
// }

/**
 * Takes a malloced void pointer to a data_t struct and frees it at the end of the function call.
 * 
 * Returns a void pointer equal to the result of reduce_safe() that was initialized on the heap. The value needs to be freed.
*/
void *routine(void *arg) {
    // Lock this section to prevent the value of arg from changing
    // Copy over the arg values
    data_t *data = (data_t *) arg;
    size_t list_len = data->list_len;
    int *list = data->list;
    reducer reduce_func = data->reduce_func;
    int base_case = data->base_case;

    // Call reduce() on the given variables
    int *result = (int *) malloc(sizeof(int));
    if (result == NULL) {
        perror("Malloc error\n");
        exit(1);
    }
    *result = reduce(list, list_len, reduce_func, base_case);
    pthread_exit(result);
}

// static pthread_mutex_t n;

#define LOG_RATIO 20

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    // I want to make this recursive
    if (num_threads == 1) {
        return reduce(list, list_len, reduce_func, base_case);
    }

    int *rec_list = malloc(sizeof(int) * list_len);
    int *next_list;
    size_t i;
    for (i = 0; i < list_len; i++) {
        rec_list[i] = list[i];
    }
    size_t rec_list_len = list_len;
    size_t max_threads = num_threads;
    do {
        /**
         * Ideally, num_threads = rec_list_len / LOG_RATIO. That way we would be able
         * to call reduce_func() and reduce rec_list_len by a factor of LOG_RATIO each time.
         * 
         * This would theoretically allow us to reach log(n) time
         */

        // If rec_list_len / LOG_RATIO < num_threads, then we only need to open rec_list_len / LOG_RATIO threads.
        max_threads = fmax(fmin(max_threads, (rec_list_len + (LOG_RATIO - 1)) / LOG_RATIO), 1);

        // Initialize pthread_t
        pthread_t *threads = calloc(max_threads, sizeof(pthread_t));

        /**
         * Given a list of length rec_list_len, divide rec_list_len by max_threads
         * and round up to find the size of each sub-list to send to a thread
         */
        size_t sub_size = rec_list_len / max_threads;
        // printf("Sub Size: %zu\n", sub_size);

        // len = 10, max_threads = 3, sub_size = 4
        // len = 9, max_threads = 3, sub_size = 3

        // Create a pointer to the list pointer and loop through it, incrementing 'cur_list' by 'sub_size', 'max_threads' times.
        int *cur_list = rec_list;
        data_t *sub_data;
        size_t i;
        for (i = 0; i < max_threads; i++) {
            // Create the sub_data variable and fill in its rec_list, replacing 'rec_list' with 'cur_list' and 'rec_list_len' with 'sub_size'    
            // On the last function call, set rec_list_len to equal rec_list_len % sub_size. If rec_list_len % sub_size == 0, then break
            sub_data = malloc(sizeof(data_t));
            if (i == max_threads - 1) {
                sub_data->list_len = sub_size + rec_list_len % max_threads;
            } else {
                sub_data->list_len = sub_size;
            }
            sub_data->list = cur_list;
            sub_data->reduce_func = reduce_func;
            sub_data->base_case = base_case;
            
            // Create a new thread
            if (pthread_create(&(threads[i]), NULL, routine, sub_data) != 0) {
                perror("pthread_create() error\n");
                exit(1);
            }

            // Increment cur_list at the end
            cur_list += sub_size;

            // Safety measure
            sub_data = NULL;
        }
        // cur_list will point to an invalid memory address at the end. It has been set to NULL for safety
        cur_list = NULL;

        // Create the next_list to hold the returned values
        next_list = calloc(max_threads, sizeof(int));

        // Rejoin the threads in another for loop
        void *return_val;
        for (i = 0; i < max_threads; i++) {
            if (pthread_join(threads[i], &return_val) != 0) {
                perror("pthread_join() error\n");
                exit(1);
            }
            // Set next_list[i] equal to the result returned by the thread
            int *result = (int *) return_val;
            next_list[i] = *result;

            // Free return_val, set return_val to NULL, and set result to NULL
            free(return_val);
            return_val = NULL;
            result = NULL;
        }

        // Free the values that we are no longer using
        free(threads);
        threads = NULL;
        
        free(rec_list);
        rec_list = NULL;

        // Update the recursive values
        rec_list = next_list;
        rec_list_len = max_threads;
        // printf("List Length: %zu\n", rec_list_len);
    } while (rec_list_len > LOG_RATIO);

    int final_value = reduce(rec_list, rec_list_len, reduce_func, base_case);

    // Free the rec_list one last time
    free(rec_list);
    rec_list = NULL;

    return final_value;
}