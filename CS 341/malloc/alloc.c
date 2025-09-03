/**
 * malloc
 * CS 341 - Spring 2024
 */
// Code reused from Fall 2023
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// 16 * 16 * 16 * 16
#define HEAP_INIT_SIZE 4096     // 16 * 16 * 16
#define GROWTH_RATIO 2          // The heap's memory allocation grows at this rate
#define SIZE_FACTOR 16           // All blocks of memory are scaled to be a factor of this number

#define BLOCK_SIZE (round_up(sizeof(block), SIZE_FACTOR))
#define TAG_SIZE (round_up(sizeof(tag), SIZE_FACTOR))
#define META_DATA_SIZE (BLOCK_SIZE + TAG_SIZE)

/**
 * A basic block structure for storing memory.
 * All blocks will be stored adjacent to each other, so that a 'next' pointer is unnecessary
*/
typedef struct block {
    bool is_free;               // Records if the block of memory is free
    struct block *prev_free;           // If the block is free, 'prev_free' points to the previous free block
    struct block *next_free;           // If the block is free, 'next_free' points to the next free block
    size_t user_size;           // The size of the allocated memory
} block;

/**
 * A tag included at the end of a block of allocated memory to help with backtracking.
*/
typedef struct tag {
    size_t user_size;           // The size of the allocated memory. It must be a multiple of SIZE_FACTOR
    // block *tag_end[0];             // A pointer to the end of the memory used by the meta data struct, 'tag'
} tag;

static block *head = NULL;      // The head of a list that will be managing blocks of memory on the 'heap'
static void *end = NULL;        // The end of the last element in the linked list of the 'heap'
static size_t heap_size = 0;    // The current size of the 'heap'

static block *free_head = NULL; // The head of a linked list that contains all free blocks of memory
static int free_total = 0;

/**
 * Given a 'number', it returns after rounding up to make 'number' a multiple of 'factor'.
*/
size_t round_up(size_t number, size_t factor) {
    // This makes use of the fact that integer division discards the remainder
    return ((number + factor - 1) / factor) * factor;
}

/**
 * Returns the address of the previous block in the heap.
 * Returns NULL if block_ptr == head
 * 
 * NOTE: This assumes the previous tag stores the correct user_size
*/
block *get_prev_block(block *block_ptr) {
    if (block_ptr == head) {
        return NULL;
    }
    tag *prev_tag = (tag *)((void *)block_ptr - TAG_SIZE);
    size_t prev_user_size = prev_tag->user_size;
    block *prev_block = (block *)((void *)prev_tag - prev_user_size - BLOCK_SIZE);
    return prev_block;
}

/**
 * Returns the end of the given 'block_ptr'.
 * Throws an error if block_ptr == NULL.
 * 
 * NOTE: This assumes block_ptr stores the correct user_size
*/
void *get_next_block(block *block_ptr) {
    if (block_ptr == NULL) {
        perror("get_next_block(): block_ptr == NULL\n");
        exit(1);
    }
    return (void *)block_ptr + block_ptr->user_size + META_DATA_SIZE;
}

/**
 * Gets the tag ptr of the given 'block_ptr'.
 * Throws an error if block_ptr == NULL.
 * 
 * NOTE: This assumes block_ptr stores the correct user_size
*/
tag *get_block_tag(block *block_ptr) {
    if (block_ptr == NULL) {
        perror("get_block_tag(): block_ptr == NULL\n");
        exit(1);
    }
    return (tag *)((void *)block_ptr + BLOCK_SIZE + block_ptr->user_size);
}

/**
 * Given a 'block_ptr', returns the 'user_ptr' contained within the block of memory, rounding the address to the nearest SIZE_FACTOR
 * Throws an error if block_ptr == NULL.
*/
void *get_user_ptr(block *block_ptr) {
    if (block_ptr == NULL) {
        perror("get_user_ptr(): block_ptr == NULL\n");
        exit(1);
    }
    // Rounds up the address of the block_ptr to always be a multiple of SIZE_FACTOR
    return (void *)block_ptr + BLOCK_SIZE;
}

/**
 * Checks if the old block_ptr has enough space to store a second block, after initializing a block of size 'block_size'
 * 
 * Returns a pointer to the start of the second block if there's enough memory available.
 * Returns NULL otherwise.
 * 
 * @param block_ptr A block pointer
 * @param requested_size The requested total_size of the smaller block
 * 
 * @note It is up to the caller to initialize the values of the first and second block
*/
block* can_split(block *block_ptr, size_t requested_size) {
    size_t old_block_size = block_ptr->user_size + META_DATA_SIZE;
    if (old_block_size < requested_size) {
        perror("can_split(): old_block_size < requested_size\n");
        exit(1);
    }
    if (old_block_size - requested_size > META_DATA_SIZE + SIZE_FACTOR) {
        return (block *)((void *)block_ptr + requested_size);
    }
    return NULL;
}

/**
 * Inserts block_ptr into the new linked list of free block pointers
 * 
 * @note Throws an error if 'block_ptr' == NULL or block_ptr != free
 * @note Does not change free_total
*/
void insert_free_block(block *block_ptr) {
    if (block_ptr == NULL) {
        perror("insert_free_block(): block_ptr == NULL\n");
        exit(1);
    }
    if (!block_ptr->is_free) {
        perror("insert_free_block(): block_ptr->is_free != true\n");
        exit(1);
    }
    free_total++;
    if (free_head == NULL) {
        free_head = block_ptr;
        return;
    }
    
    // If block_ptr is before all existing free blocks
    if ((size_t)block_ptr < (size_t)free_head) {
        block_ptr->next_free = free_head;                       // block   ->   head
        if (free_head != NULL) {
            free_head->prev_free = block_ptr;                       // block   <-   head
        }
        free_head = block_ptr;                                  // head   <-->  block
        return;
    }
    block *cur_block = free_head;
    while (cur_block->next_free != NULL) {
        block *next_block = cur_block->next_free;
        if ((size_t)cur_block < (size_t)block_ptr && (size_t)block_ptr < (size_t)next_block) {
            block_ptr->next_free = cur_block->next_free;    // cur      block  ->   next
            cur_block->next_free = block_ptr;               // cur  ->  block       next
            block_ptr->prev_free = cur_block;               // cur  <-  block       next
            block_ptr->next_free->prev_free = block_ptr;    // cur      block  <-   next
            return;
        }
        cur_block = cur_block->next_free;
    }
    // If the function exits, that means block_ptr is after all existing free blocks
    cur_block->next_free = block_ptr;
    block_ptr->prev_free = cur_block;
}

/**
 * Removes block_ptr from the linked list of free block pointers
 * 
 * @note Throws an error if 'block_ptr' == NULL or block_ptr != free
 * @note Does not change free_total
*/
void remove_free_block(block *block_ptr) {
    if (block_ptr == NULL) {
        perror("remove_free_block(): block_ptr == NULL\n");
        exit(1);
    }
    if (!block_ptr->is_free) {
        perror("remove_free_block(): block_ptr->is_free != true\n");
        exit(1);
    }
    free_total--;
    if (block_ptr->prev_free != NULL) {
        block_ptr->prev_free->next_free = block_ptr->next_free;     //  prev  ----  block  -->  next
    }
    if (block_ptr == free_head) {
        free_head = block_ptr->next_free;
    }
    if (block_ptr->next_free != NULL) {
        block_ptr->next_free->prev_free = block_ptr->prev_free;    //  prev  <---  block  ---  next
    }
    block_ptr->prev_free = NULL;                                //  prev   X-   block       next
    block_ptr->next_free = NULL;                                //  prev        block  -X   next
}

/**
 * Merges two blocks, assuming the first block's address is before the second block's address.
 * 
 * @note Throws an error if first_block == NULL or second_block == NULL
*/
void merge_free_free(block *first_block, block *second_block) {
    // // Scrub the value of the old prev_block's tag
    // tag *prev_tag = get_block_tag(prev_block);
    // prev_tag->user_size = 0;

    // Set 'first_block->user_size' to the combined 'user_size' + META_DATA_SIZE
    first_block->user_size = first_block->user_size + second_block->user_size + META_DATA_SIZE;
    first_block->is_free = true;
    first_block->next_free = second_block->next_free;
    if (second_block->next_free != NULL) {
        second_block->next_free->prev_free = first_block;
    }
    remove_free_block(second_block);

    // // Scrub the value of the old block_ptr
    // block_ptr->is_free = true;
    // block_ptr->user_size = 0;

    // Set the new 'tag_ptr' to the new 'user_size'
    tag *tag_ptr = get_block_tag(first_block);
    tag_ptr->user_size = first_block->user_size;
}

/**
 * @brief Reinitialize an existing block of memory, splitting the blocks if second_block isn't NULL.
 * 
 * @param first_block The first block of memory
 * @param second_block The second block of memory. Can be NULL.
 * @param total_size The total size of both blocks of memory
 * 
 * @note init_old_block assumes that the memory has already been freed.
 * @note init_old_block assumes that first_block < second_block
*/
void init_old_block(block *first_block, block* second_block, size_t total_size) {
    if (first_block == NULL) {
        perror("init_old_block(): first_block == NULL\n");
        exit(1);
    }
    remove_free_block(first_block);
    first_block->is_free = false;
    // If second_block != NULL, then split the blocks and initialize the second_block as free
    if (second_block != NULL) {
        if ((size_t)first_block > (size_t)second_block) {
            perror("init_old_block(): first_block > second_block\n");
            exit(1);
        }
        first_block->user_size = ((size_t)second_block - (size_t)first_block) - META_DATA_SIZE;
        tag *first_tag = get_block_tag(first_block);
        first_tag->user_size = first_block->user_size;

        // second_block is free
        second_block->is_free = true;
        second_block->next_free = NULL;
        second_block->prev_free = NULL;
        second_block->user_size = total_size - (first_block->user_size + META_DATA_SIZE) - META_DATA_SIZE;
        tag *second_tag = get_block_tag(second_block);
        second_tag->user_size = second_block->user_size;

        insert_free_block(second_block);
    }
}

/**
 * Initializes a block of memory given a 'meta_ptr' and the 'total_size' that was allocated for the meta_ptr.
 * 
 * @note If the block was initialized on the end of the list, sets end equal to the end of the 'meta_ptr'
 * @note init_block() assumes that block_size is a factor of SIZE_FACTOR
*/
block *init_block(void *meta_ptr, size_t block_size) {
    if (block_size < META_DATA_SIZE) {
        perror("init_block(): block_size < META_DATA_SIZE.\n");
        exit(1);
    }
    size_t user_size = block_size - META_DATA_SIZE;
    block *block_ptr = (block *)meta_ptr;
    // Check if the block of memory has already been initialized
    if (block_ptr != end) {
        // If the old size can be split into two chunks of memory, split it and reinitialize the blocks of memory
        block *second_block = can_split(block_ptr, block_size);
        size_t total_size = block_ptr->user_size + META_DATA_SIZE;
        init_old_block(block_ptr, second_block, total_size);
    } else {
        // Otherwise, initialize the new memory at the end of the list
        block_ptr->is_free = false;
        block_ptr->next_free = NULL;
        block_ptr->prev_free = NULL;
        block_ptr->user_size = user_size;
        tag *block_tag = get_block_tag(block_ptr);
        block_tag->user_size = user_size;
        
        end = get_next_block(block_ptr);
    }
    return block_ptr;
}

// Checks if head is NULL
bool valid_head() {
    return head != NULL;
}

/**
 * Initializes 'head' to sbrk(0)
 * Initializes 'end' to 'head'
 * Initializes 'heap_size' to HEAP_INIT_SIZE
 * Calls sbrk(HEAP_INIT_SIZE)
*/
void init_head() {
    head = sbrk(0);
    end = head;
    heap_size = HEAP_INIT_SIZE;
    sbrk(HEAP_INIT_SIZE);
}

/**
 * Increases the size of the heap by a factor of GROWTH_RATIO.
 * 
 * Returns 0 on success and -1 on failure
*/
int expand_heap() {
    size_t additional_size = heap_size * GROWTH_RATIO - heap_size;
    if (sbrk(additional_size) == (void *)-1) {
        perror("sbrk failed\n");
        return -1;
    }
    heap_size += additional_size;
    return 0;
}

/**
 * Checks if the program will run out of memory after allocating 'total_size' bytes to ptr
*/
bool out_of_memory(size_t total_size) {
    return ((size_t)end + total_size - (size_t)head >= heap_size);
}

/**
 * Look through the list of memory, starting from 'head', to find the next available block of memory that fits the required 'total_size'
 * 
 * NOTE: find_next_fit will return a block that 'is_free' or is the 'end' of the list
*/
void *find_next_fit(size_t total_size) {
    // If the heap is empty
    size_t user_size = total_size - META_DATA_SIZE;
    // Otherwise, initialize cur_block to head and cycle through it until a suitable gap within the linked list is found
    int unseen_frees = free_total;
    block *cur_block = free_head;
    while (cur_block != NULL) {
        if ((size_t) cur_block > (size_t) end) {
            perror("find_next_fit(): cur_block out of bounds\n");
            exit(1);
        }
        if (!cur_block->is_free) {
            perror("find_next_fit(): cur_block is not free\n");
            exit(1);
        }
        // ICheck if it can hold the requested memory
        unseen_frees--;
        if (cur_block->user_size >= user_size) {
            return cur_block;
        }

        // Otherwise, continue to the next block in the list (free or not free)
        cur_block = cur_block->next_free;
    }
    // If unseen_frees > 0 but we've reached the end of the list, then there's an error
    if (unseen_frees != 0) {
        fprintf(stderr, "find_next_fit(): unseen_frees == %d. free_total == %d\n", unseen_frees, free_total);
        exit(1);
    }

    // If no gap is found, return the address at the end of the last block
    return end;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    // implement malloc!
    // Initializes 'head' if it hasn't been initialized
    if (!valid_head()) {
        init_head();
    }
    // Get the 'total_size' of the new block of memory, including the META_DATA_SIZE, and rounding up to the nearest SIZE_FACTOR
    size_t total_size = round_up(size, SIZE_FACTOR) + META_DATA_SIZE;

    // Find next free block of memory that can hold 'total_size'
    void *meta_ptr = find_next_fit(total_size);

    // Expand the heap if more memory is needed
    while(meta_ptr == end && out_of_memory(total_size)) {
        if (expand_heap() == -1) {
            return NULL;
        }
    }

    // Initialize the block of memory
    block *block_ptr = init_block(meta_ptr, total_size);

    // // Insert the new block between the previous block and following block
    // insert_block(block_ptr);

    return get_user_ptr(block_ptr);
}

bool next_is_free(block *next_block) {
    return next_block != end && next_block->is_free;
}

bool prev_is_free(block *prev_block) {
    return prev_block != NULL && prev_block->is_free;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
    if (ptr == NULL) {
        return;
    }

    // Get the 'block_ptr'
    block *block_ptr = (block *)(ptr - BLOCK_SIZE);

    // Check for a double free
    if (block_ptr->is_free) {
        perror("free(): double free detected\n");
        exit(1);
    }

    // Otherwise, mark 'block_ptr' as free
    block_ptr->is_free = true;
    insert_free_block(block_ptr);

    // Get 'next_block'
    block *next_block = (block *) get_next_block(block_ptr);
    // If 'next_block' is also free, merge them
    if (next_is_free(next_block)) {
        merge_free_free(block_ptr, next_block);
    }
    // This means that block_ptr is now always followed by a non-free block.

    // Otherwise, get 'prev_block'
    block *prev_block = get_prev_block(block_ptr);
    // If 'prev_block' is also free, merge them
    if (prev_is_free(prev_block)) {
        merge_free_free(prev_block, block_ptr);
    }
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
    size_t total_user_size = num * size;
    void *user_ptr = malloc(total_user_size);

    // If malloc() fails
    if (user_ptr == NULL) {
        return NULL;
    }

    // Otherwise, set the bytes allocated for the user_ptr to 0
    memset(user_ptr, 0, total_user_size);
    return user_ptr;
}

/**
 * Merges 'next_block' into 'block_ptr'
*/
void merge_unfree_free(block *block_ptr, block *next_block) {
    block_ptr->user_size = block_ptr->user_size + next_block->user_size + META_DATA_SIZE;

    remove_free_block(next_block);
    // next_block->is_free = false;

    // Set the new 'tag_ptr' to the new 'user_size'
    tag *tag_ptr = get_block_tag(block_ptr);
    tag_ptr->user_size = block_ptr->user_size;
}

/**
 * Merges 'block_ptr' into 'prev_block'
*/
void merge_free_unfree(block *prev_block, block *block_ptr) {
    prev_block->user_size = prev_block->user_size + block_ptr->user_size + META_DATA_SIZE;

    remove_free_block(prev_block);
    prev_block->is_free = false;

    // Set the new 'tag_ptr' to the new 'user_size'
    tag *tag_ptr = get_block_tag(prev_block);
    tag_ptr->user_size = prev_block->user_size;

    // Copy over the values of block_ptr into prev_block
    memmove(get_user_ptr(prev_block), get_user_ptr(block_ptr), block_ptr->user_size);
}

/**
 * Reallocate memory block.
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!
    
    // If ptr == NULL && size == 0, undefined behavior
    if (ptr == NULL && size == 0) {
        perror("realloc(): ptr == NULL and size == 0\n");
        exit(1);
    }
    // If ptr == NULL, malloc(size)
    else if (ptr == NULL) {
        return malloc(size);
    }
    // If size == NULL, free(ptr)
    else if (size == 0) {
        free(ptr);
        return NULL;
    }

    // Idiot check
    block *block_ptr = ptr - BLOCK_SIZE;

    // First, combine the block_ptr with any nearby free blocks
    block *next_block = get_next_block(block_ptr);
    if (next_is_free(next_block)) {
        merge_unfree_free(block_ptr, next_block);
    }
    // block *prev_block = get_prev_block(block_ptr);
    // if (prev_is_free(prev_block)) {
    //     merge_free_unfree(prev_block, block_ptr);
    //     block_ptr = prev_block;
    // }

    // Check if the newly combined size exactly fits the requested size
    if (block_ptr->user_size == size) {
        return ptr;
    }

    // Then, check if the newly combined 'block_ptr' is larger than the requested size
    if (block_ptr->user_size > size) {
        free(ptr);
        void *new_ptr = malloc(size);
        // Malloc should never fail, since ptr has already been freed
        if (new_ptr == NULL) {
            perror("realloc(): new_ptr == NULL");
            exit(1);
        }
        memmove(new_ptr, ptr, (block_ptr->user_size < size) ? block_ptr->user_size : size);
        return new_ptr;
    }
    // Otherwise, allocate a new block of memory and copy over the values
    else {
        // Allocate a new size
        void *new_ptr = malloc(size);
        // If malloc() fails
        if (new_ptr == NULL) {
            return NULL;
        }

        // Otherwise, copy over the contents of 'ptr' into 'new_ptr' and free the old 'ptr'
        memmove(new_ptr, ptr, (block_ptr->user_size < size) ? block_ptr->user_size : size);
        free(ptr);

        return new_ptr;
    }
}