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
    if (*new_macro == NULL) err(errno, "*** ERROR ***\nFailed to allocate memory for a new macro");
    (*new_macro)->name = my_strdup(macro_name);
    (*new_macro)->lines = NULL;
    (*new_macro)->line_capacity = 0;
    (*new_macro)->line_count = 0;

    return STATUS_OK;
}

status insert_line_to_macro(macro *mac, char *line) {

    if (mac->lines == NULL) {
        mac->lines = (char **)malloc(sizeof(char *) * INITIAL_MACRO_CAPACITY);
        if (mac->lines == NULL) err(errno, "*** ERROR ***\nFailed to reallocate memory for macro lines");
        mac->line_capacity = INITIAL_MACRO_CAPACITY;
    }

    if (mac->line_count == mac->line_capacity) {
        mac->line_capacity = mac->line_count + 1;
        mac->lines = (char **)realloc(mac->lines, mac->line_capacity * sizeof(char *));
        if (mac->lines == NULL) {
            err(errno, "*** ERROR ***\nFailed to allocate memory for macro lines");
        }
    }

    mac->lines[mac->line_count] = my_strdup(line);
    mac->line_count++;
    return STATUS_OK;
}

/**
 * @brief Create a new macro table object
 *
 * @return macro_table*
 */
macro_table *create_macro_table() {

    macro_table *m_table = (macro_table *)malloc(sizeof(macro_table));
    if (m_table == NULL) {
        err(errno, "*** ERROR ***\nFailed to allocate memory for macro table");
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
        printf("*** ERROR ***\nTrying to insert macro to a NULL table. Exiting...");
        return STATUS_ERROR;
    }

    if (table->macros == NULL) {
        table->macros = (macro **)calloc(INITIAL_MACRO_TABLE_CAPACITY, sizeof(macro *));
        if (table->macros == NULL) return STATUS_ERROR_MEMORY_ALLOCATION;
        table->capacity = INITIAL_MACRO_TABLE_CAPACITY;
    }


    if (table->macro_count == table->capacity) {
        table->capacity = table->macro_count + 1;
        table->macros = (macro **)realloc(table->macros, table->capacity * sizeof(macro *));
        if (table->macros == NULL) {
            macro_table_destructor(&table);
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    table->macros[table->macro_count] = macr;
    table->macro_count++;
    return STATUS_OK;
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
void macro_destructor(macro **mac) {
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
void macro_table_destructor(macro_table **table) {
    int i;

    if (table == NULL) return;

    for (i = 0; i < (*table)->macro_count; i++) {
        macro_destructor(&((*table)->macros[i]));
    }
    free((*table)->macros);
    free((*table));
    *table = NULL;
}

status print_macro_lines(macro *mac) {
    int index;

    if (mac->lines == NULL) {


    }

    for (index = 0; index < mac->line_count; index++)
        printf("Macro %s: Line #%d: %s\n", mac->name, index + 1, mac->lines[index]);

    printf("Finished printing lines for macro %s\n", mac->name);
    printf("--------------------------------------------------------------\n");
    return STATUS_OK;

}

status print_macro_table(macro_table *table) {
    macro *current_macro;
    int index;
    if (table->macros == NULL) return STATUS_ERROR_MACRO_TABLE_IS_EMPTY;
    printf("\n--------------------------------------------------------------\n");
    printf("Printing macro table...\n");

    for (index = 0;index < table->macro_count;index++) {
        current_macro = table->macros[index];
        printf("\n***Macro name: %s***\n\n", current_macro->name);
        if (print_macro_lines(current_macro) != STATUS_OK) {
            printf("*** ERROR ***\nError while printing macro %s Exiting...", current_macro->name);
            return STATUS_ERROR;
        }
    }
    printf("Done printing macro table");
    printf("\n--------------------------------------------------------------\n");
    return STATUS_OK;

}