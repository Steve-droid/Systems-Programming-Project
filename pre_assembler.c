
#include "pre_assembler.h"
#define DEFINE_SEQUENCE_LEN 5

static bool is_macro_definition(char *line) {
    return (strncmp(line, "macr ", DEFINE_SEQUENCE_LEN) == 0);
}

static bool is_macro_call(char *line, macro_table *table) {
    char macro_name[MAX_MACRO_NAME_LENGTH] = { '\0' };

    /*
    * We use 'sscanf' to read input from a specific string
    * 'sscanf' will read the first word from 'line' into 'macro_name'
    */
    sscanf(line, "%s", macro_name);
    return (find_macro_in_table(table, macro_name) != NULL);
}

static status add_macro_to_table(char *macro_name, FILE *as_file, macro_table *table) {
    macro *new_macro;
    char line[BUFSIZ];
    status result = create_macro(macro_name, &new_macro);

    if (result != STATUS_OK) return result;

    while (fgets(line, sizeof(line), as_file)) {
        if (strncmp(line, "endmacr", 7) == 0) break;
        result = insert_line_to_macro(new_macro, line);
        if (result != STATUS_OK) return result;
    }

    printf("Adding macro: %s\n", macro_name);
    return insert_macro_to_table(table, new_macro);
}

static status expand_macro(char *macro_name, FILE *am_file, macro_table *table) {
    int i;
    macro *m = find_macro_in_table(table, macro_name);
    if (m == NULL) return STATUS_ERROR_MACRO_NOT_FOUND;
    for (i = 0;i < m->line_count;i++) fprintf(am_file, "%s", m->lines[i]);
    return STATUS_OK;
}

status pre_assemble(char *as_filename, char *am_filename, macro_table *m_table) {
    FILE *as_file = NULL, *am_file = NULL;
    char line[BUFSIZ] = { '\0' };
    char first_word[MAX_LINE_LENGTH] = { '\0' };
    char macro_name[MAX_LINE_LENGTH] = { '\0' };
    macro *macroname_found_flag = NULL;


    if (remove_whitespace(as_filename) != STATUS_OK) {
        free(as_filename);
        free(am_filename);
        return STATUS_ERROR;
    }

    as_file = fopen(as_filename, "r");
    if (as_file == NULL) {
        fprintf(stderr, "Error opening file %s\n", as_filename);
        free(as_filename);
        free(am_filename);
        return STATUS_ERROR_OPEN_SRC;
    }

    am_file = fopen(am_filename, "w");
    if (am_file == NULL) {
        fprintf(stderr, "Error opening file %s\n", am_filename);
        fclose(as_file);
        free(as_filename);
        free(am_filename);
        return STATUS_ERROR_OPEN_DEST;
    }

    /* Primary parsing loop */
    while (fgets(line, sizeof(line), as_file)) {
        line[strcspn(line, "\n")] = '\0'; /* Null terminate line */
        sscanf(line, "%s", first_word); /* Extract first word in the line */

        if (is_macro_call(line, m_table)) {
            if (expand_macro(first_word, am_file, m_table) != STATUS_OK) {
                printf("Error while expanding macro %s. Exiting...\n", first_word);
                exit(EXIT_FAILURE);
            }
        }
        else if (is_macro_definition(line)) {
            sscanf(line, "macr %s", macro_name); /* Extract 2nd word wich is the macro name */

            /* Check if definition is valid */
            macroname_found_flag = find_macro_in_table(m_table, macro_name);
            if (macroname_found_flag != NULL) {
                fclose(as_file);
                fclose(am_file);
                free(as_filename);
                free(am_filename);
                return STATUS_ERROR_MACRO_REDEFINITION;
            }

            /* Is a valid definition of a new macro */
            add_macro_to_table(macro_name, as_file, m_table);
        }
        /* Neither a macro call or definition */
        else fprintf(am_file, "%s\n", line); /* Write the line to the .am file */
    }

    fclose(as_file);
    fclose(am_file);


    return STATUS_OK;

}
