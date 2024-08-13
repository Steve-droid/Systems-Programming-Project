
#include "pre_assembler.h"

static int is_macro_definition(syntax_state *state) {
    return (strncmp(state->buffer, "macr ", DEFINE_SEQUENCE_LEN) == 0);
}

static int is_macro_call(char *line, macro_table *table) {
    char macro_name[MAX_MACRO_NAME_LENGTH] = { '\0' };

    /*
    * We use 'sscanf' to read input from a specific string
    * 'sscanf' will read the first word from 'line' into 'macro_name'
    */
    sscanf(line, "%s", macro_name);
    return (get_macro(table, macro_name) != NULL);
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

    if (insert_macro_to_table(table, new_macro) != STATUS_OK) {
        macro_destructor(&new_macro);
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

static status expand_macro(char *macro_name, FILE *am_file, macro_table *table) {
    int i;
    macro *m = get_macro(table, macro_name);
    if (m == NULL) return STATUS_ERROR_MACRO_NOT_FOUND;
    for (i = 0;i < m->line_count;i++) fprintf(am_file, "%s", m->lines[i]);
    return STATUS_OK;
}

static status pre_assemble(char *as_filename, char *am_filename, macro_table *m_table, keyword *keyword_table) {
    FILE *as_file = NULL, *am_file = NULL;
    char first_word[MAX_LINE_LENGTH] = { '\0' };
    char macro_name[MAX_LINE_LENGTH] = { '\0' };
    macro *macroname_found_flag = NULL;
    status result = STATUS_ERROR;
    syntax_state *state = NULL;
    char *token = NULL;


    state = create_syntax_state();

    if (state == NULL) {
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    state->line_number++;
    state->as_filename = as_filename;
    state->am_filename = am_filename;

    if (remove_whitespace_from_file(as_filename) != STATUS_OK) {
        printf("Error while removing whitespace from %s\nExiting...\n", as_filename);
        destroy_syntax_state(&state);
        return STATUS_ERROR;
    }

    as_file = my_fopen(as_filename, "r");
    if (as_file == NULL) {
        printf("Error: Could not open file called: %s\nExiting...", as_filename);
        destroy_syntax_state(&state);
        return STATUS_ERROR_OPEN_SRC;
    }

    am_file = my_fopen(am_filename, "w");
    if (am_file == NULL) {
        printf("Error: Could not open file called: %s\nExiting...", am_filename);
        fclose(as_file);
        destroy_syntax_state(&state);
        return STATUS_ERROR_OPEN_DEST;
    }

    while (fgets(state->buffer, MAX_LINE_LENGTH, as_file)) {/* Primary parsing loop */

        state->line_number++;

        state->buffer_without_offset = state->buffer;

        state->buffer[strcspn(state->buffer, "\n")] = '\0'; /* Null terminate line */
        sscanf(state->buffer, "%s", first_word); /* Extract first word in the line */

        /* Check if the current line is a macro call. If so, copy the expanded macro lines to the .am file*/
        if (is_macro_call(state->buffer, m_table)) {
            if (expand_macro(first_word, am_file, m_table) != STATUS_OK) {
                printf("Error: line %d: '%s':", state->line_number, state->buffer_without_offset);
                printf("Could not expand the macro '%s' \n", first_word);
                fclose(as_file);
                fclose(am_file);
                macro_table_destructor(&m_table);
                destroy_syntax_state(&state);
                return STATUS_ERROR;
            }
        }

        /* Check if the current line is a macro definition. If so, add macro to the macro table */
        else if (is_macro_definition(state)) {

            state->buffer += strlen("macro");

            token = state->buffer;
            token = strchr(token, ' ');
            while (token != NULL && (*token) != '\n' && (*token) != '\0') {
                if (!isspace(*token)) {
                    printf("Error in file: %s: line %d: '%s': ", state->as_filename, state->line_number, state->buffer_without_offset);
                    printf("Macro name definition contains more than one word.\n");
                    macro_table_destructor(&m_table);
                    destroy_syntax_state(&state);
                    fclose(as_file);
                    fclose(am_file);
                    return STATUS_ERROR;
                }
                token++;
            }

            /*
             Check if definition is valid.
             If the macro name is found in the macro table, exit on error- redifinition is not allowed.
             */
            macroname_found_flag = get_macro(m_table, macro_name);
            if (macroname_found_flag != NULL) {
                printf("Error: line %d: '%s':", state->line_number, state->buffer_without_offset);
                printf("Macro with the name '%s' is already defined\n", macro_name);
                macro_table_destructor(&m_table);
                destroy_syntax_state(&state);
                fclose(as_file);
                fclose(am_file);
                return STATUS_ERROR_MACRO_REDEFINITION;
            }

            if (get_keyword_by_name(keyword_table, macro_name) != NULL) {
                printf("Error in file '%s' on line %d: '%s': ", as_filename, state->line_number, state->buffer_without_offset);
                printf("Trying to define a macro with the same name as the keyword '%s'\n", macro_name);
                macro_table_destructor(&m_table);
                destroy_syntax_state(&state);
                fclose(as_file);
                fclose(am_file);
                return STATUS_ERROR_MACRO_NAMED_AS_KEYWORD;
            }

            /* Is a valid definition of a new macro */
            result = add_macro_to_table(state->buffer, as_file, m_table);

            if (result != STATUS_OK) {
                printf("Error: file: %s, line %d: '%s':", as_filename, state->line_number, state->buffer_without_offset);
                printf("Could not add macro '%s' to macro table\n", macro_name);
                macro_table_destructor(&m_table);
                destroy_syntax_state(&state);
                fclose(as_file);
                fclose(am_file);
                return result;
            }
        }
        /* Neither a macro call or definition */
        else fprintf(am_file, "%s\n", state->buffer); /* Write the line to the .am file */
        state->buffer = state->buffer_without_offset;
        reset_syntax_state(state);
    }

    /*Clean up allocated memory*/
    state->buffer_without_offset = state->buffer;
    destroy_syntax_state(&state);
    fclose(as_file);
    fclose(am_file);

    return STATUS_OK;

}

macro_table *fill_macro_table(char *am_filename, char *as_filename, keyword *keyword_table) {
    macro_table *m_table = NULL;
    status result = STATUS_ERROR;

    m_table = create_macro_table();
    if (m_table == NULL) {
        printf("Error: Could not create macro table. Exiting...\n");
        return NULL;
    }


    result = pre_assemble(as_filename, am_filename, m_table, keyword_table);

    if (result != STATUS_OK) {
        return NULL;
    }



    return m_table;
}
