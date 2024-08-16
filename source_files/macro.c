#include "macro.h"
#include "text_util.h"
/**
 * @brief Create a new macro object
 *
 * @param name The name of the macro
 * @return macro*
 */
status create_macro(char *macro_name, macro **new_macro) {
    *new_macro = (macro *)malloc(sizeof(macro));
    if (*new_macro == NULL)
        return failure;
    (*new_macro)->name = my_strdup(macro_name);
    (*new_macro)->lines = NULL;
    (*new_macro)->line_capacity = 0;
    (*new_macro)->line_count = 0;

    return success;
}

status insert_line_to_macro(macro *mac, char *line) {

    if (mac->lines == NULL) {
        mac->lines = (char **)malloc(sizeof(char *) * INITIAL_MACRO_CAPACITY);
        if (mac->lines == NULL)
            return failure;
        mac->line_capacity = INITIAL_MACRO_CAPACITY;
    }

    if (mac->line_count == mac->line_capacity) {
        mac->line_capacity = mac->line_count + 1;
        mac->lines = (char **)realloc(mac->lines, mac->line_capacity * sizeof(char *));
        if (mac->lines == NULL) {
            return failure;
        }
    }

    mac->lines[mac->line_count] = my_strdup(line);
    if (mac->lines[mac->line_count] == NULL) {
        return failure;
    }
    mac->line_count++;
    return success;
}

/**
 * @brief Create a new macro table object
 *
 * @return macro_table*
 */
macro_table *create_macro_table() {

    macro_table *m_table = (macro_table *)malloc(sizeof(macro_table));
    if (m_table == NULL) {
        return NULL;
    }

    m_table->macros = NULL;
    m_table->macro_count = 0;
    m_table->capacity = 0;

    return m_table;
}

/**
 * @brief Insert a macro into the macro table
 *
 * @param table The macro table to insert the macro into
 * @param macro The macro to insert
 */
status insert_macro_to_table(macro_table *table, macro *macr) {
    if (table == NULL) {
        return failure;
    }

    if (table->macros == NULL) {
        table->macros = (macro **)calloc(INITIAL_MACRO_TABLE_CAPACITY, sizeof(macro *));
        if (table->macros == NULL) return failure;
        table->capacity = INITIAL_MACRO_TABLE_CAPACITY;
    }


    if (table->macro_count == table->capacity) {
        table->capacity = table->macro_count + 1;
        table->macros = (macro **)realloc(table->macros, table->capacity * sizeof(macro *));
        if (table->macros == NULL) {
            destroy_macro_table(&table);
            return failure;
        }
    }

    table->macros[table->macro_count] = macr;
    table->macro_count++;
    return success;
}

/**
 * @brief Get a macro from the macro table
 *
 * @param table The macro table to get the macro from
 * @param name The name of the macro to get
 * @return macro* If macro with matching name is found
 * @return NULL if not found
 */
macro *get_macro(macro_table *table, char *name) {
    int index;
    if (table == NULL) return NULL;

    for (index = 0;index < table->macro_count;index++)
        if (strcmp(table->macros[index]->name, name) == 0)
            return table->macros[index];
    return NULL;
}

/**
 *@brief Get the macro table object
 *
 * @return macro_table*
 */
macro_table *get_macro_table(void) {

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
void destroy_macro(macro **mac) {
    int index;

    for (index = 0;index < (*mac)->line_count;index++) {
        free((*mac)->lines[index]);
        (*mac)->lines[index] = NULL;
    }
    free((*mac)->lines);
    free((*mac)->name);
    free((*mac));
    *mac = NULL;
}

/**
 * @brief Destroy a macro table object
 *
 * @param table The macro table to destroy
 */
void destroy_macro_table(macro_table **table) {
    int i;

    if (table == NULL) return;

    if ((*table) && (*table)->macros) {

        for (i = 0; i < (*table)->macro_count; i++) {
            destroy_macro(&((*table)->macros[i]));

        }

        free((*table)->macros);
    }

    free((*table));
    *table = NULL;
}

