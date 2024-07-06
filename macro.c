#include "macro.h"
/**
 * @brief Create a new macro object
 *
 * @param name The name of the macro
 * @return macro*
 */
macro *create_macro(char *macro_name) {

    macro *new_macro = (macro *)malloc(sizeof(macro));
    if (new_macro == NULL) {
        err(errno, "Failed to allocate memory for macro");
    }

    new_macro->name = strdup(macro_name);
    new_macro->lines = NULL;
    new_macro->line_capacity = 0;
    new_macro->line_count = 0;

    return new_macro;
}

void insert_line_to_macro(macro *mac, char *line) {

    if (mac->lines == NULL) {
        mac->lines = (char **)malloc(sizeof(char *) * MIN_LINE_AMOUNT);
        if (mac->lines == NULL) {
            err(errno, "Failed to allocate memory for macro lines");
        }
        mac->line_capacity = MIN_LINE_AMOUNT;
        *(mac->lines) = strdup(line);
        mac->line_count = 1;
    }

    if (mac->line_count == mac->line_capacity) {
        mac->line_capacity = mac->line_count + 1;
        mac->lines = (char **)realloc(mac->lines, mac->line_capacity * sizeof(char *));
        if (mac->lines == NULL) {
            err(errno, "Failed to reallocate memory for macro lines");
        }
        mac->lines[mac->line_count] = strdup(line);
        mac->line_count++;
    }
}

/**
 * @brief Create a new macro table object
 *
 * @return macro_table*
 */
macro_table *create_macro_table() {

    macro_table *m_table = (macro_table *)malloc(sizeof(macro_table));
    if (m_table == NULL) {
        err(errno, "Failed to allocate memory for macro table");
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
void insert_macro_to_table(macro_table *table, macro *macr) {

    if (table->macros == NULL) {
        table->macros = (macro **)malloc(sizeof(macro *) * MIN_MACRO_AMOUNT);
        if (table->macros == NULL) {
            err(errno, "Failed to allocate memory for macro table");
        }
        table->capacity = MIN_MACRO_AMOUNT;
        *(table->macros) = create_macro(macr->name);
        table->macro_count = 1;
    }


    if (table->macro_count == table->capacity) {
        table->capacity = table->macro_count + 1;
        table->macros = (macro **)realloc(table->macros, table->capacity * sizeof(macro *));
        if (table->macros == NULL) {
            err(errno, "Failed to allocate memory for macro object");
        }
        table->macros[table->macro_count] = macr;
        table->macro_count++;
    }
}

/**
 * @brief Get a macro from the macro table
 *
 * @param table The macro table to get the macro from
 * @param name The name of the macro to get
 * @return macro*
 */
macro *find_macro_in_table(macro_table *table, char *name) {
    if (table == NULL) return NULL;

    macro *_macro;
    int index;

    for (_macro = *table->macros, index = 0;(_macro != NULL) && index < table->capacity;_macro++, index++) {
        if (strcmp(_macro->name, name) == 0) return _macro;
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
void macro_destructor(macro *mac) {
    char *tmp = NULL;
    while (mac->lines != NULL && mac->line_count > 0) {
        tmp = *mac->lines;
        mac->lines++;
        mac->line_count--;
        free(tmp);
    }

    free(mac->name);
    free(mac);
}

/**
 * @brief Destroy a macro table object
 *
 * @param table The macro table to destroy
 */
void macro_table_destructor(macro_table *table) {

    macro *tmp = NULL;
    while (table->macros != NULL && table->macro_count > 0) {
        tmp = *table->macros;
        table->macros++;
        table->macro_count--;
        macro_destructor(tmp);
    }

    free(table);
}