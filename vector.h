#ifndef VECTOR_H
#define VECTOR_H
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#define VECTOR_INITIAL_CAPACITY 5

typedef struct vector {

    void **data;                                /* Pointer to an array of pointers to any given item */
    size_t  size;                               /* Number of elements in the vector */
    size_t  capacity;                           /* Amount of memory allocated for the vector */
    void *(*constructor)(const void *source);   /* Pointer to the constructor function */
    void  (*destructor)(void *to_destroy);      /* Pointer to the destructor function  */
} vector;

/**
 *@brief Create a vector object
 * The vector object is a dynamic array that can store any type of data
 * The constructor and destructor functions are used to create and destroy the elements in the vector
 * The construction and destruction functions should be defined by the user for the specific type of data
 * This struct is meant to simulate a generic vector class in C++(or a vector list in Java)
 * This struct is used as an API for the entire project, and relies on the ability to create generic data structures in C
 * @param constructor
 * @param destructor
 * @return vector*
 */
vector *vector_create(void *(*constructor)(const void *source), void (*destructor)(void *to_destroy));

void vector_insert(vector *vec, const void *element);

/**
 *@brief Get the element at the specified index
 *
 * @param vec The vector to get the element from
 * @param index The index of the element to get
 * @return void*
 */
void *vector_get(vector *vec, size_t index);

/**
 *@brief Remove the element at the specified index
 *
 * @param vec The vector to remove the element from
 * @param index The index of the element to remove
 */
void vector_remove_element_at_index(vector *vec, size_t index);

/**
 *@brief Remove the specified element from the vector
 *
 * @param vec Pointer to the vector to remove the element from
 * @param element Pointer to the element to remove
 */
void vector_remove_element(vector *vec, const void *element);

/**
 *@brief Destroy the vector and free all of its memory
 * Do this by calling the destructor function on each element in the vector
 * @param vec The vector to destroy
 */
void vector_destroy(vector *vec);

#endif