
#include "pre_assembler.h"

bool is_macro_definition(syntax_state *state) {
    return (strncmp(state->buffer, "macr ", DEFINE_SEQUENCE_LEN) == 0);
}

bool is_macro_call(char *line, macro_table *table) {
    char macro_name[MAX_MACRO_NAME_LENGTH] = { '\0' };

    /*
    * We use 'sscanf' to read input from a specific string
    * 'sscanf' will read the first word from 'line' into 'macro_name'
    */
    sscanf(line, "%s", macro_name);
    return (get_macro(table, macro_name) != NULL);
}

status process_new_macro(syntax_state *state, char *macro_name, FILE *as_file, macro_table *table) {
    macro *new_macro;
    char line[BUFSIZ];
    char *tmp;
    bool end = false;
    int line_count = state->line_number;
    int original_line_count = state->line_number;
    status result = create_macro(macro_name, &new_macro);

    if (result != success) return result;

    /* Scan the lines the macro expands to */
    while (fgets(line, sizeof(line), as_file)) {

        /* Keep scanning until 'endmacr' is found */
        if (strncmp(line, "endmacr", 7) == 0) {
            end = true;
            break;
        }

        line_count++;

        /* Insert the line to the macro */
        result = insert_line_to_macro(new_macro, line);
        if (result != success) {
            destroy_macro(&new_macro);
            return result;
        }
    }
    /* Check if the macro definition is closed with 'endmacr' */
    if (end == false) {
        /* If not, print an error message, free the allocated memory and return failure */
        destroy_macro(&new_macro);
        return failure;
    }


    /* Check if there are any characters after 'endmacr' */
    tmp = trim_whitespace(line);
    if (tmp[7] != '\n' && tmp[7] != '\0') {
        tmp = state->buffer_without_offset;
        state->buffer_without_offset = line;
        state->line_number = line_count;
        print_syntax_error(state, e53_ext_chars_after_endmacr);
        state->buffer_without_offset = tmp;
        state->line_number = original_line_count;
        destroy_macro(&new_macro);
        return failure;
    }
    if (insert_macro_to_table(table, new_macro) != success) {
        destroy_macro(&new_macro);
        return failure;
    }

    return success;
}

status expand_macro(char *macro_name, FILE *am_file, macro_table *table) {
    int i;
    macro *m = get_macro(table, macro_name);
    if (m == NULL) return failure;
    for (i = 0;i < m->line_count;i++) fprintf(am_file, "%s", m->lines[i]);
    return success;
}

macro_table *pre_assemble(char *as_filename, char *am_filename, keyword *keyword_table) {
    FILE *as_file = NULL;
    FILE *am_file = NULL;
    char first_word[MAX_LINE_LENGTH] = { '\0' };
    char macro_name[MAX_LINE_LENGTH] = { '\0' };
    macro *macroname_found_flag = NULL;
    macro_table *m_table = NULL;
    status result = failure;
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

        state->buffer = trim_whitespace(state->buffer); /* Remove leading and trailing whitespace */

        if (state->buffer[0] == '\0' || state->buffer[0] == '\n' || state->buffer[0] == ';') continue; /* Skip empty lines and comments */


        sscanf(state->buffer, "%s", first_word); /* Extract first word in the line */

        /* Check if the current line is a macro call. If so, copy the expanded macro lines to the .am file*/
        if (is_macro_call(state->buffer, m_table)) {
            if (expand_macro(first_word, am_file, m_table) != success) {
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

            /* If the definition line is valid, parse the macro itself and add it to the macro table */
            result = process_new_macro(state, macro_name, as_file, m_table);

            if (result != success) {
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



