
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

static status add_macro_to_table(syntax_state *state, char *macro_name, FILE *as_file, macro_table *table) {
    macro *new_macro;
    char line[BUFSIZ];
    char *tmp;
    bool end = false;
    int line_count = state->line_number;
    int original_line_count = state->line_number;
    status result = create_macro(macro_name, &new_macro);

    if (result != STATUS_OK) return result;

    while (fgets(line, sizeof(line), as_file)) {
        if (strncmp(line, "endmacr", 7) == 0) {
            end = true;
            break;
        }
        line_count++;
        result = insert_line_to_macro(new_macro, line);
        if (result != STATUS_OK) return result;
    }
    if (end == false) {
        destroy_macro(&new_macro);
        return STATUS_ERROR;
    }


    tmp = trim_whitespace(line);
    if (tmp[7] != '\n' && tmp[7] != '\0') {
        tmp = state->buffer_without_offset;
        state->buffer_without_offset = line;
        state->line_number = line_count;
        print_syntax_error(state, e53_ext_chars_after_endmacr);
        state->buffer_without_offset = tmp;
        state->line_number = original_line_count;
        destroy_macro(&new_macro);
        return STATUS_ERROR;
    }
    if (insert_macro_to_table(table, new_macro) != STATUS_OK) {
        destroy_macro(&new_macro);
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

macro_table *pre_assemble(char *as_filename, char *am_filename, keyword *keyword_table) {
    FILE *as_file = NULL;
    FILE *am_file = NULL;
    char first_word[MAX_LINE_LENGTH] = { '\0' };
    char macro_name[MAX_LINE_LENGTH] = { '\0' };
    macro *macroname_found_flag = NULL;
    macro_table *m_table = NULL;
    status result = STATUS_ERROR;
    syntax_state *state = NULL;
    char *token = NULL;
    char *token_without_offset = NULL;

    m_table = create_macro_table();
    if (m_table == NULL) {
        return NULL;
    }

    state = create_syntax_state();

    if (state == NULL) {
        return NULL;
    }

    state->line_number++;
    state->as_filename = as_filename;
    state->am_filename = am_filename;

    if (remove_whitespace_from_file(as_filename) != STATUS_OK) {
        quit_pre_assembler(&state, &m_table, NULL, NULL);
        return NULL;
    }

    as_file = my_fopen(as_filename, "r");
    if (as_file == NULL) {
        quit_pre_assembler(&state, &m_table, NULL, NULL);
        return NULL;
    }

    am_file = my_fopen(am_filename, "w");
    if (am_file == NULL) {
        quit_pre_assembler(&state, &m_table, as_file, NULL);
        return NULL;
    }

    while (fgets(state->buffer, MAX_LINE_LENGTH, as_file)) {/* Primary parsing loop */

        state->line_number++;

        state->buffer_without_offset = state->buffer;

        state->buffer[strcspn(state->buffer, "\n")] = '\0'; /* Null terminate line */


        sscanf(state->buffer, "%s", first_word); /* Extract first word in the line */

        /* Check if the current line is a macro call. If so, copy the expanded macro lines to the .am file*/
        if (is_macro_call(state->buffer, m_table)) {
            if (expand_macro(first_word, am_file, m_table) != STATUS_OK) {
                state->tmp_arg = first_word;
                print_syntax_error(state, e55_macro_not_found);
                quit_pre_assembler(&state, &m_table, am_file, as_file);
                return NULL;
            }
        }

        /* Check if the current line is a macro definition. If so, add macro to the macro table */
        else if (is_macro_definition(state)) {

            state->buffer += strlen("macro");
            strcpy(macro_name, state->buffer);

            token_without_offset = my_strdup(state->buffer);
            token = strchr(token_without_offset, ' ');
            while (token != NULL && (*token) != '\n' && (*token) != '\0') {
                if (!isspace(*token)) {
                    print_syntax_error(state, e56_macro_name_not_valid);
                    free(token_without_offset);
                    quit_pre_assembler(&state, &m_table, am_file, as_file);
                    return NULL;
                }
                token++;
            }

            free(token_without_offset);

            /*
             Check if definition is valid.
             If the macro name is found in the macro table, exit on error- redifinition is not allowed.
             */
            macroname_found_flag = get_macro(m_table, macro_name);
            if (macroname_found_flag != NULL) {
                state->tmp_arg = macro_name;
                print_syntax_error(state, e54_macr_already_defined);
                quit_pre_assembler(&state, &m_table, am_file, as_file);
                return NULL;
            }

            if (get_keyword_by_name(keyword_table, macro_name) != NULL) {
                state->tmp_arg = macro_name;
                print_syntax_error(state, e57_macroname_same_as_keyword);
                quit_pre_assembler(&state, &m_table, am_file, as_file);
                return NULL;
            }

            /* Is a valid definition of a new macro */
            result = add_macro_to_table(state, macro_name, as_file, m_table);

            if (result != STATUS_OK) {
                quit_pre_assembler(&state, &m_table, am_file, as_file);
                return NULL;
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

    return m_table;

}



