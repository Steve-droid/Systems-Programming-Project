#include "vector.h"

/**
 *@brief Create an empty vector object
 *
 * @param constructor Pointer to the constructor function of the vector
 * @param destructor Pointer to the destructor function of the vector
 * @return vector*
 */
vector *vector_create(void *(*constructor)(const void *source), void (*destructor)(void *to_destroy)) {

    vector *vec = (vector *)malloc(sizeof(vector));
    if (vec == NULL) {
        err(errno, "Failed to allocate memory for vector");
    }

    vec->data = (void **)malloc(VECTOR_INITIAL_CAPACITY * sizeof(void *));
    if (vec->data == NULL) {
        err(errno, "Failed to allocate memory for vector data");
    }

    vec->size = 0;
    vec->capacity = VECTOR_INITIAL_CAPACITY;
    vec->constructor = constructor;
    vec->destructor = destructor;

    return vec;
}

/**
 *@brief Insert an element into the vector
 *
 * @param vec A pointer to the vector to insert the element into
 * @param element A pointer to the element to insert
 */
void vector_insert(vector *vec, const void *element) {

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->data = (void **)realloc(vec->data, vec->capacity * sizeof(void *));
        if (vec->data == NULL) {
            err(errno, "Failed to reallocate memory for vector data");
        }
    }

    vec->data[vec->size] = vec->constructor(element);
    vec->size++;
}

/**
 *@brief Get the element at the specified index
 *
 * @param vec A pointer to the vector to get the element from
 * @param index The index of the element to get
 * @return void*
 */
void *vector_get(vector *vec, size_t index) {

    if (index >= vec->size) {
        errno = EINVAL;
        err(errno, "Index out of bounds");
    }

    return vec->data[index];
}

/**
 *@brief Remove the element at the specified index
 *
 * @param vec A pointer to the vector to remove the element from
 * @param index The index of the element to remove
 */
void vector_remove_element_at_index(vector *vec, size_t index) {

    if (index >= vec->size) {
        errno = EINVAL;
        err(errno, "Index out of bounds");
    }

    vec->destructor(vec->data[index]);

    for (size_t i = index; i < vec->size - 1; i++) {
        vec->data[i] = vec->data[i + 1];
    }

    vec->size--;
}

void vector_remove_element(vector *vec, const void *element) {

    for (size_t i = 0; i < vec->size; i++) {
        if (vec->data[i] == element) {
            vector_remove_element_at_index(vec, i);
            return;
        }
    }

    errno = EINVAL;
    err(errno, "Element not found in vector");
}

/**
 *@brief Destroy the vector and free all of its memory
 * Do this by calling the destructor function on each element in the vector
 * The destructor function for the elements should be defined by whoever created the elements stored in the vector
 * @param vec A pointer to the vector to destroy
 */
void vector_destroy(vector *vec) {

    for (size_t i = 0; i < vec->size; i++) {
        vec->destructor(vec->data[i]);
    }

    free(vec->data);
    free(vec);
}

/*----------------Example-----------------*/


/**
 *@brief Example constructor function for an integer
 *
 * @param source Pointer to the integer to copy
 * @return void*
 */
void *int_constructor(const void *source) {
    int *copy = (int *)malloc(sizeof(int));
    if (copy) {
        *copy = *(const int *)source;
    }
    return copy;
}

/**
 *@brief Example destructor function for an integer
 *
 * @param source A pointer to the integer to destroy
 *
 * @return void*
 */
void int_destructor(void *to_destroy) {
    free(to_destroy);
}

void test_vector() {
    vector *vec = vector_create(int_constructor, int_destructor);
    int *x = NULL;
    int *y = NULL;
    int *z = NULL;
    int a = 5;
    int b = 10;
    int c = 15;

    vector_insert(vec, &a);
    vector_insert(vec, &b);
    vector_insert(vec, &c);

    printf("***Printing elements in vector***:\n");
    for (size_t i = 0; i < vec->size; i++) {
        int *element = (int *)vector_get(vec, i);
        printf("%d\n", *element);
    }
    printf("***Vector printed***\n");

    printf("Testing vector insert- Inserting 21,22,23,24 to the vector\n");
    for (size_t i = 20; i < 25; i++) {
        vector_insert(vec, &i);
    }

    printf("***Printing elements in vector (after insert)***:\n");
    for (size_t i = 0; i < vec->size; i++) {
        int *element = (int *)vector_get(vec, i);
        printf("%d\n", *element);
    }

    printf("***Vector printed***\n");

    printf("Testing vector remove function\n");
    printf("Removing elements 5,15,23\n");

    x = vector_get(vec, 0);
    y = vector_get(vec, 2);
    z = vector_get(vec, 6);

    vector_remove_element(vec, x);
    vector_remove_element(vec, y);
    vector_remove_element(vec, z);


    printf("***Printing elements in vector***:\n");
    for (size_t i = 0; i < vec->size; i++) {
        int *element = (int *)vector_get(vec, i);
        printf("%d\n", *element);
    }
    printf("***Vector printed***\n");
    vector_destroy(vec);
}
/*----------------Example-----------------*/
