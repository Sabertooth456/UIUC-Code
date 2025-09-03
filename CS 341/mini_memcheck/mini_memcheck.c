/**
 * mini_memcheck
 * CS 341 - Spring 2024
 */

// Code copied from Fall 2023

/**
 * Note: There is an error that involves an incorrect amount of memory being recorded by realloc.
 * This might mean that the proper version of realloc should record *all* newly allocated bits when the
 * size of the pointer is changed.
*/

#include <stdio.h>
#include <string.h>

#include <stdbool.h>

#include "mini_memcheck.h"

#define META_DATA_SIZE (sizeof(meta_data))


meta_data *head = NULL;
size_t total_memory_requested;
size_t total_memory_freed;
size_t invalid_addresses;

/**
 * Starting from the head pointer, traverses and returns the pointer that 'next' points to the 'target' pointer.
 * 
 * If the pointer is not found, returns NULL;
 * 
 * NOTE: This does not check if the 'head' pointer stores the 'target' pointer
*/
meta_data *find_parent_ptr(void *target) {
    // NOTE: Target points to the address META_DATA_SIZE bytes after the current meta_data pointer. Be careful.
    if (head == NULL) {
        return NULL;
    }
    // Get the head
    meta_data *cur_ptr = head;
    // Loop through the current pointer until the end has been reached
    while (cur_ptr->next != NULL) {
        // If the next meta_data pointer is storing the address of the target pointer, return the current pointer
        if (((void *)cur_ptr->next) + META_DATA_SIZE == target) {
            return cur_ptr;
        }
        cur_ptr = cur_ptr->next;
    }
    return NULL;
}

/**
 * Checks if the 'head' pointer stores the 'target' pointer
*/
bool is_head_ptr(void *target) {
    if (head == NULL) {
        return false;
    }
    return (((void *)head) + META_DATA_SIZE) == target;
}

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    // Idiot check
    if (request_size == 0) {
        // Undefined
        return NULL;
    }

    // Initialize the meta_data struct with an additional requested_size bytes
    meta_data *next_ptr = malloc(META_DATA_SIZE + request_size);

    // Fill in the meta_data variables
    next_ptr->request_size = request_size;
    next_ptr->filename = filename;
    next_ptr->instruction = instruction;
    next_ptr->next = head;
    head = next_ptr;

    // Update total_memory_requested
    total_memory_requested += request_size;

    // Get and return the user_ptr
    void *user_ptr = ((void *)next_ptr) + META_DATA_SIZE;
    return user_ptr;
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
    // Idiot check
    if (num_elements == 0 || element_size == 0) {
        // Undefined
        return NULL;
    }

    // Get the request_size by multiplying num_elements with element_size
    size_t request_size = num_elements * element_size;

    // Initialize the meta_data struct with an additional requested_size bytes
    meta_data *next_ptr = calloc(META_DATA_SIZE + request_size + sizeof(void *), 1);
    // This code breaks if I do "calloc(META_DATA_SIZE + request_size, 1);"
    // But it works if I do "calloc(META_DATA_SIZE + request_size + 1, 1);"
    // I don't understand why. Ask at office hours?

    // Fill in the meta_data variables
    next_ptr->request_size = request_size;
    next_ptr->filename = filename;
    next_ptr->instruction = instruction;
    next_ptr->next = head;
    head = next_ptr;

    // Update total_memory_requested
    total_memory_requested += request_size;

    // Get and return the user_ptr
    void *user_ptr = ((void *)head) + META_DATA_SIZE;
    return user_ptr;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    // Idiot check
    if (payload == NULL && request_size == 0) {
        // Undefined
        return NULL;
    } else if (payload == NULL) {
        return mini_malloc(request_size, filename, instruction);
    } else if (request_size == 0) {
        mini_free(payload);
        return NULL;
    }

    // Initialize the meta_data struct with an additional requested_size bytes
    if (is_head_ptr(payload)) {
        meta_data* old_head = head;
        head = realloc(((void *)head), request_size + META_DATA_SIZE);
        if (head == NULL) {
            return NULL;
        }
        
        // If the new request_size is larger than the old request size, increase total_memory_requested
        if (request_size > head->request_size) {
            if (head == old_head) {
                total_memory_requested += request_size - head->request_size;
            } else {
                total_memory_requested += request_size;
                total_memory_freed += head->request_size;
            }
            head->request_size = request_size;
        }
        // If the new request_size is smaller than the old request, increase total_memory_freed
        else if (request_size < head->request_size) {
            if (head == old_head) {
                total_memory_freed += head->request_size - request_size;
            } else {
                total_memory_requested += request_size;
                total_memory_freed += head->request_size;
            }
            head->request_size = request_size;
        }
        return ((void *)head) + META_DATA_SIZE;
    } else {
        // Sanity check
        meta_data *parent_ptr = find_parent_ptr(payload);
        if (parent_ptr == NULL) {
            invalid_addresses++;
            return NULL;
        }

        meta_data *payload_ptr = parent_ptr->next;

        meta_data *old_payload_ptr = payload_ptr;
        payload_ptr = realloc(((void *)payload_ptr), request_size + META_DATA_SIZE);
        if (payload_ptr == NULL) {
            return NULL;
        }
        // realloc will either:
        //  initialize new memory, copy over the contents, and free the old address
        if (parent_ptr->next != payload_ptr) {
            parent_ptr->next = payload_ptr;
        }
        //  or reallocate space for the same memory address, in which case, nothing else needs to be done
        
        // If the new request_size is larger than the old request size, increase total_memory_requested
        if (request_size > payload_ptr->request_size) {
            if (payload_ptr == old_payload_ptr) {
                total_memory_requested += request_size - payload_ptr->request_size;
            } else {
                total_memory_requested += request_size;
                total_memory_freed += payload_ptr->request_size;
            }
            payload_ptr->request_size = request_size;
        }
        // If the new request_size is smaller than the old request, increase total_memory_freed
        else if (request_size < payload_ptr->request_size) {
            if (payload_ptr == old_payload_ptr) {
                total_memory_freed += payload_ptr->request_size - request_size;
            } else {
                total_memory_requested += request_size;
                total_memory_freed += payload_ptr->request_size;
            }
            payload_ptr->request_size = request_size;
        }
        // If they are equal, then nothing was risked and nothing was gained.
        
        // Get and return the user_ptr
        return ((void *)payload_ptr) + META_DATA_SIZE;
    }
}

void mini_free(void *payload) {
    // your code here
    // Idiot check
    if (payload == NULL) {
        return;
    }
    // If 'head' is the target pointer, free() it and assign head->next as the new head
    if (is_head_ptr(payload)) {
        meta_data *temp = head->next;

        total_memory_freed += head->request_size;
        free(head);
        head = temp;
        return;
    }

    // Find the parent node
    meta_data* parent_ptr = find_parent_ptr(payload);
    if (parent_ptr == NULL) {
        invalid_addresses++;
        return;
    }

    // Find the payload node and the child of the payload node
    meta_data* payload_ptr = parent_ptr->next;
    meta_data* child_ptr = payload_ptr->next;

    // Free the payload node
    total_memory_freed += payload_ptr->request_size;
    free(payload_ptr); payload_ptr = NULL;
    parent_ptr->next = child_ptr;
}