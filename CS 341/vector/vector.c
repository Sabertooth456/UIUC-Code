/**
 * vector
 * CS 341 - Spring 2024
 */
#include "vector.h"
#include <assert.h>

struct vector {
    /* The function callback for the user to define the way they want to copy
     * elements */
    copy_constructor_type copy_constructor;

    /* The function callback for the user to define the way they want to destroy
     * elements */
    destructor_type destructor;

    /* The function callback for the user to define the way they a default
     * element to be constructed */
    default_constructor_type default_constructor;

    /* Void pointer to the beginning of an array of void pointers to arbitrary
     * data. */
    void **array;

    /**
     * The number of elements in the vector.
     * This is the number of actual objects held in the vector,
     * which is not necessarily equal to its capacity.
     */
    size_t size;

    /**
     * The size of the storage space currently allocated for the vector,
     * expressed in terms of elements.
     */
    size_t capacity;
};

/**
 * IMPLEMENTATION DETAILS
 *
 * The following is documented only in the .c file of vector,
 * since it is implementation specfic and does not concern the user:
 *
 * This vector is defined by the struct above.
 * The struct is complete as is and does not need any modifications.
 *
 * The only conditions of automatic reallocation is that
 * they should happen logarithmically compared to the growth of the size of the
 * vector inorder to achieve amortized constant time complexity for appending to
 * the vector.
 *
 * For our implementation automatic reallocation happens when -and only when-
 * adding to the vector makes its new  size surpass its current vector capacity
 * OR when the user calls on vector_reserve().
 * When this happens the new capacity will be whatever power of the
 * 'GROWTH_FACTOR' greater than or equal to the target capacity.
 * In the case when the new size exceeds the current capacity the target
 * capacity is the new size.
 * In the case when the user calls vector_reserve(n) the target capacity is 'n'
 * itself.
 * We have provided get_new_capacity() to help make this less ambigious.
 */

static size_t get_new_capacity(size_t target) {
    /**
     * This function works according to 'automatic reallocation'.
     * Start at 1 and keep multiplying by the GROWTH_FACTOR untl
     * you have exceeded or met your target capacity.
     */
    size_t new_capacity = 1;
    while (new_capacity < target) {
        new_capacity *= GROWTH_FACTOR;
    }
    return new_capacity;
}

vector *vector_create(copy_constructor_type copy_constructor,
                      destructor_type destructor,
                      default_constructor_type default_constructor) {
    // your code here
    // Casting to void to remove complier error. Remove this line when you are
    // ready.
    // (void)INITIAL_CAPACITY;
    // (void)get_new_capacity;

    vector *new_vec = malloc(sizeof(vector));

    if (copy_constructor == NULL) {
        new_vec->copy_constructor = shallow_copy_constructor;
    } else {
        new_vec->copy_constructor = copy_constructor;
    }

    if (destructor == NULL) {
        new_vec->destructor = shallow_destructor;
    } else {
        new_vec->destructor = destructor;
    }

    if (default_constructor == NULL) {
        new_vec->default_constructor = shallow_default_constructor;
    } else {
        new_vec->default_constructor = default_constructor;
    }

    new_vec->array = calloc(INITIAL_CAPACITY, sizeof(void *));
    new_vec->size = 0;
    new_vec->capacity = INITIAL_CAPACITY;

    return new_vec;
}

void free_segment(vector *this, size_t start, size_t end) {
    size_t i;
    for (i = start; i < end; i++) {
        this->destructor(vector_get(this, i));
        *vector_at(this, i) = NULL;
    }
}

void vector_destroy(vector *this) {
    assert(this);
    // your code here
    free_segment(this, 0, vector_size(this));
    free(this->array);
    this->array = NULL;
    this->size = 0;
    this->capacity = 0;
    free(this);
}

void **vector_begin(vector *this) {
    return this->array + 0;
}

void **vector_end(vector *this) {
    return this->array + vector_size(this);
}

size_t vector_size(vector *this) {
    assert(this);
    // your code here
    return this->size;
}

void vector_resize(vector *this, size_t n) {
    assert(this);
    // your code here
    if (n < vector_size(this)) {
        free_segment(this, n, vector_size(this)); // Clear the erased memory
        this->size = n;
        return;
    }
    if (n > vector_capacity(this)) {
        vector_reserve(this, n);
    }
    if (n > vector_size(this)) {
        size_t i = vector_size(this);
        this->size = n;
        for (; i < n; i++) {    // Change the values of the array pointers to equal the default constructor
            *vector_at(this, i) = this->default_constructor();
        }
    }
}

size_t vector_capacity(vector *this) {
    assert(this);
    // your code here
    return this->capacity;
}

bool vector_empty(vector *this) {
    assert(this);
    // your code here
    return !vector_size(this);     // vector_size(this) is True if it's not empty, so !vector_size(this) is True if it's empty
}

void vector_reserve(vector *this, size_t n) {
    assert(this);
    // your code here
    if (n > vector_capacity(this)) {
        size_t new_cap = get_new_capacity(n);
        void **new_array = realloc(this->array, new_cap * sizeof(void *));
        assert(new_array);      // realloc returns a NULL pointer if it fails
        this->array = new_array;
        this->capacity = new_cap;
    }
}

void **vector_at(vector *this, size_t position) {
    assert(this);
    // your code here
    assert(position < vector_size(this));
    return this->array + position;
}

void vector_set(vector *this, size_t position, void *element) {
    assert(this);
    // your code here
    assert(position < vector_size(this));
    // // Idiot check
    // if (position >= vector_size(this)) {
    //     printf("vector_set(): position >= vector_size(this). You're an idiot.\n");
    //     return;
    // }

    void **elem = vector_at(this, position);
    this->destructor(*elem);
    *elem = (*(this->copy_constructor))(element);
}

void *vector_get(vector *this, size_t position) {
    assert(this);
    // your code here
    assert(position < vector_size(this));
    return *vector_at(this, position);
}

/**
 * Returns a pointer to the first element in the vector.
 *
 * Calling this function on an empty container causes undefined behavior.
 *
 * http://www.cplusplus.com/reference/vector/vector/front/
 */
void **vector_front(vector *this) {
    assert(this);
    // your code here
    return vector_at(this, 0);
}

void **vector_back(vector *this) {
    // your code here
    return vector_at(this, vector_size(this) - 1);
}

void vector_push_back(vector *this, void *element) {
    assert(this);
    // your code here
    if (vector_size(this) == this->capacity) {     // If the vector is full
        vector_reserve(this, this->capacity + 1);
    }
    this->size++;
    *vector_back(this) = (*(this->copy_constructor))(element);
}

void vector_pop_back(vector *this) {
    assert(this);
    // your code here
    assert(!vector_empty(this));
    // // Idiot check
    // if (vector_empty(this)) {
    //     printf("vector_pop_back(): vector_empty(this) == True. You're an idiot.\n");
    //     return;
    // }
    void **elem = vector_at(this, vector_size(this) - 1);
    this->destructor(*elem);
    *elem = NULL;
    this->size--;
}

void vector_insert(vector *this, size_t position, void *element) {
    assert(this);
    // your code here
    assert(position <= vector_size(this));
    // Idiot check
    if (position == vector_size(this)) {
        vector_push_back(this, element);
        return;
    }

    if (vector_size(this) == this->capacity) {     // If the vector is full
        vector_reserve(this, this->capacity + 1);
    }

    this->size++;
    size_t i;
    void *new_elem = (*(this->copy_constructor))(element);
    void *old_elem;
    for (i = position; i < vector_size(this) - 1; i++) {
        old_elem = vector_get(this, i);
        *vector_at(this, i) = new_elem;
        new_elem = old_elem;
    }
    *vector_back(this) = new_elem;
}

void vector_erase(vector *this, size_t position) {
    assert(this);
    assert(position < vector_size(this));
    // your code here
    assert(!vector_empty(this));
    // // Idiot check
    // if (vector_empty(this)) {
    //     printf("vector_empty(this) == True. You're an idiot.\n");
    //     return;
    // }

    // Destroys the element at the specified position
    this->destructor(vector_get(this, position));

    // Copes the rest of the array to fill in the lost space
    size_t i;
    for (i = position; i < vector_size(this) - 1; i++) {
        *vector_at(this, i) = vector_get(this, i+1);
    }
    
    // Remove the last index
    *vector_at(this, vector_size(this) - 1) = NULL;
    this->size--;
}

void vector_clear(vector *this) {
    // your code here
    size_t i;
    for (i = 0; i < vector_size(this); i++) {
        this->destructor(vector_get(this, i));
        *vector_at(this, i) = NULL;
    }
    this->size = 0;
}