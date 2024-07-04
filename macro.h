#ifndef MACRO_H_
#define MACRO_H_
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vector.h"

#define as_extention ".as"
#define am_extention ".am"
#define MAX_MACRO_NAME_LENGTH 31
#define MAX_MACRO_AMOUNT 100
#define MAX_LINE_LENGTH 81

typedef enum macro_state {
    definition, empty_line, line_other, macro_redefinition_error,
    illegal_definition, illegeal_end_macro, call, illegal_call
}macro_state;

typedef macro *macro_pointer;

typedef struct macro {
    vector *lines;      /* Vector of strings- the lines that the macro expands to */
    char *name;         /* The name of the macro */
    int line_count;     /* Number of lines in the macro */
}macro;

typedef struct macro_table {
    vector *table;      /* Vector of macro pointers */
    int size;           /* Number of macros in the table */
}macro_table;

/**
 * @brief Create a new macro object
 *
 * @param name The name of the macro
 * @return macro*
 */
macro *macro_constructor(char *name);

/**
 * @brief Create a new macro table object
 *
 * @return macro_table*
 */
macro_table *macro_table_constructor();

/**
 * @brief Insert a macro into the macro table
 *
 * @param table The macro table to insert the macro into
 * @param macro The macro to insert
 */
void insert_macro(macro_table *table, macro *macro);

/**
 * @brief Get a macro from the macro table
 *
 * @param table The macro table to get the macro from
 * @param name The name of the macro to get
 * @return macro*
 */
macro *get_macro(macro_table *table, char *name);


/**
 *@brief Get the macro table object
 *
 * @return macro_table*
 */
macro_table *get_macro_table();

/**
 * @brief Destroy a macro object
 *
 * @param macro The macro to destroy
 */
void macro_destructor(macro *macro);

/**
 * @brief Destroy a macro table object
 *
 * @param table The macro table to destroy
 */
void macro_table_destructor(macro_table *table);

#endif