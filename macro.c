#include "macro.h"
/**
 * @brief Create a new macro object
 *
 * @param name The name of the macro
 * @return macro*
 */
macro *macro_constructor(char *macro_name) {

    macro *new_macro = (macro *)malloc(sizeof(macro));
    if (new_macro == NULL) {
        err(errno, "Failed to allocate memory for macro");
    }

    new_macro->lines = vector_create((void *(*)(const void *))strdup, free);
    new_macro->name = strdup(macro_name);
    new_macro->line_count = 0;

    return new_macro;
}

/**
 * @brief Create a new macro table object
 *
 * @return macro_table*
 */
macro_table *macro_table_constructor() {

    macro_table *table = (macro_table *)malloc(sizeof(macro_table));
    if (table == NULL) {
        err(errno, "Failed to allocate memory for macro table");
    }

    table->table = vector_create((void *(*)(const void *))macro_constructor, (void (*)(void *))macro_destructor);
    table->size = 0;

    return table;
}

/**
 * @brief Insert a macro into the macro table
 *
 * @param table The macro table to insert the macro into
 * @param macro The macro to insert
 */
void insert_macro(macro_table *table, macro *macro) {

    vector_insert(table->table, macro);
    table->size++;
}

/**
 * @brief Get a macro from the macro table
 *
 * @param table The macro table to get the macro from
 * @param name The name of the macro to get
 * @return macro*
 */
macro *get_macro(macro_table *table, char *name) {

    for (int i = 0; i < table->size; i++) {
        macro *current = (macro *)vector_get(table->table, i);
        if (strcmp(current->name, name) == 0) {
            return current;
        }
    }

    return NULL;
}

/**
 *@brief Get the macro table object
 *
 * @return macro_table*
 */
macro_table *get_macro_table() {

    static macro_table *table = NULL;
    if (table == NULL) {
        table = create_macro_table();
    }

    return table;
}

/**
 * @brief Destroy a macro object
 *
 * @param macro The macro to destroy
 */
void macro_destructor(macro *macro) {

    free(macro->name);
    vector_destroy(macro->lines);
    free((void *)macro);
}

/**
 * @brief Destroy a macro table object
 *
 * @param table The macro table to destroy
 */
void macro_table_destructor(macro_table *table) {

    for (int i = 0; i < table->size; i++) {
        macro *current = (macro *)vector_get(table->table, i);
        destroy_macro(current);
    }

    vector_destroy(table->table);
    free(table);
}