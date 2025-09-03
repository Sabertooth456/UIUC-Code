/**
 * critical_concurrency
 * CS 341 - Spring 2024
 */
// Code reused from Fall 2023

#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    size_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    size_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    // Initialize the basic values
    queue *new_queue = malloc(sizeof(queue));
    new_queue->head = NULL;
    new_queue->tail = NULL;
    new_queue->size = 0;
    new_queue->max_size = max_size;

    // Initialize the pthreads
    new_queue->m = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    new_queue->cv = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    return new_queue;
}

void queue_destroy(queue *this) {
    /* Your code here */
    // Destroy the basic values
    queue_node *cur_node = this->head;
    while (cur_node != NULL) {
        queue_node *next_node = cur_node->next;
        // free(cur_node->data);
        // cur_node->data = NULL;
        free(cur_node);
        cur_node = next_node;
        this->size--;
    }
    this->head = NULL;
    this->tail = NULL;
    if (this->size != 0) {
        printf("queue_destroy() : Remaining nodes in queue- %zu\n", this->size);
    }

    // Destroy the pthreads. Might be unnecessary
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->cv);

    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    if (this->max_size > 0 && this->size == this->max_size) {
        // printf("Waiting to pull\n");
        while (this->size == this->max_size) {
            // Wait until an element has been removed from the queue
            pthread_cond_wait(&this->cv, &this->m);
        }
        // printf("Ready to pull\n");
    }
    // When there's room, add a new element to the tail
    if (this->head == NULL) {
        queue_node *new_node = malloc(sizeof(queue_node));
        new_node->data = data;
        new_node->next = NULL;
        this->head = new_node;
        this->tail = new_node;
    } else {
        queue_node *new_node = malloc(sizeof(queue_node));
        new_node->data = data;
        new_node->next = NULL;
        this->tail->next = new_node;
        this->tail = new_node;
    }

    // Increase size and broadcast the update.
    this->size++;
    pthread_cond_signal(&this->cv);

    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    if (this->size == 0) {
        while (this->size == 0) {
            // Wait until an element has been added to the queue
            pthread_cond_wait(&this->cv, &this->m);
        }
    }
    // Get the front node and it's data
    queue_node *front_node = this->head;
    void *front_data = front_node->data;

    // Shift the head to point to the next element
    this->head = this->head->next;
    if (this->head == NULL) {
        this->tail = NULL;
    }

    // Free the front node
    free(front_node);
    front_node = NULL;

    // Decrease size and broadcast the update.
    this->size--;
    pthread_cond_signal(&this->cv);

    pthread_mutex_unlock(&this->m);
    return front_data;
}