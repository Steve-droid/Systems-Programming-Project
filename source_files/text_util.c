#include "text_util.h"
#define PADDING 4

/**
 *@brief Create a syntax state object
 * Dynamically allocate memory for a new syntax state object and initialize its fields.
 * @return syntax_state* The newly created syntax state object
 */
syntax_state *create_syntax_state(void) {
    char *buffer = NULL;
    syntax_state *state = (syntax_state *)calloc(1, sizeof(syntax_state));

    if (state == NULL)
        return NULL;

    buffer = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if (buffer == NULL) {
        free(state);
        state = NULL;
        return NULL;
    }

    state->buffer_without_offset = buffer;
    state->k_table = NULL;
    state->index = -1;
    state->am_filename = NULL;
    state->as_filename = NULL;
    state->continue_reading = false;
    state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
    state->buffer = buffer;
    state->line_number = -1;
    state->_inst = NULL;
    state->label_name_detected = false;
    state->label_key = -1;
    state->comma = false;
    state->whitespace = false;
    state->null_terminator = false;
    state->new_line = false;
    state->minus_sign = false;
    state->plus_sign = false;
    state->end_of_argument_by_space = false;
    state->first_quatiotion_mark = false;
    state->last_quatiotion_mark = false;
    state->digit = false;
    state->is_data = false;
    state->is_string = false;
    return state;
}

/**
 *@brief Reset the syntax state object
 * Use this function to reset the syntax state object by clearing the buffer and setting the index to -1.
 * It also resets the flags that indicate the presence of certain characters in the buffer.
 * We reset the state each time we start processing a new instruction.
 * @param state
 */
void reset_syntax_state(syntax_state *state) {
    size_t i = 0;


    if (state->buffer_without_offset != NULL) {
        state->buffer = state->buffer_without_offset;
        for (i = 0;i < MAX_LINE_LENGTH;i++) {
            state->buffer[i] = '\0';
        }

        state->buffer_without_offset = NULL;
    }

    else {
        for (i = 0;i < MAX_LINE_LENGTH;i++) {
            state->buffer[i] = '\0';
        }
    }


    state->index = -1;
    state->_label = NULL;

    state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
    state->_inst = NULL;
    state->continue_reading = false;
    state->label_name_detected = false;
    state->label_key = -1;
    state->comma = false;
    state->whitespace = false;
    state->null_terminator = false;
    state->new_line = false;
    state->minus_sign = false;
    state->plus_sign = false;
    state->end_of_argument_by_space = false;
    state->first_quatiotion_mark = false;
    state->last_quatiotion_mark = false;
    state->digit = false;
    state->is_data = false;
    state->is_string = false;
    state->tmp_arg = NULL;
}

/**
 *@brief Destroy the syntax state object
 * This function frees the memory allocated for the syntax state object.
 * @param state The syntax state object to destroy
 */
void destroy_syntax_state(syntax_state **state) {
    syntax_state *st = NULL;

    if (state == NULL || (*state) == NULL)
        return;

    st = (*state);
    if (st->buffer != NULL) {

        if (st->buffer_without_offset != NULL) {

            st->buffer = st->buffer_without_offset;
            st->buffer_without_offset = NULL;
            free(st->buffer);
            st->buffer = NULL;
        }
        else {
            free(st->buffer);
            st->buffer = NULL;
        }
    }

    if (st->_inst)
        st->_inst = NULL;

    if (st->k_table)
        st->k_table = NULL;

    if (st->_label)
        st->_label = NULL;

    if (st->m_table)
        st->m_table = NULL;

    if (st->l_table)
        st->l_table = NULL;

    free(st);
    st = NULL;
    (*state) = NULL;
}

/**
 *@brief Updates the syntax state according to the command key
 * Used in the lexer to update the syntax state according to the command key.
 * @param state The syntax state of the current instruction as defined in ds.h
 * @param keyword_table
 * @param command_key The key of the command to update the syntax state according to
 */
void update_command(syntax_state *state, keyword *keyword_table, int command_key) {
    if (!strcmp(keyword_table[command_key].name, ".data")) {
        state->is_data = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".string")) {
        state->is_string = true;
    }
}

/**
 *@brief Continue reading the instruction buffer
 * This function checks if there are more characters to read in the instruction buffer.
 * Used in the lexer to determine if we should continue reading the instruction buffer.
 * @param instruction_buffer The buffer containing the instruction
 * @param state The syntax state of the current instruction as defined in ds.h
 * @return status The status of the operation
 */
bool continue_reading(char *instruction_buffer, syntax_state *state) {
    size_t index = state->index;
    size_t instruction_length = strlen(instruction_buffer);
    if (index >= instruction_length) {
        return false;
    }

    /* If we reached a null terminator or a new line, break out of the loop */
    if (instruction_buffer[index] == '\0' || instruction_buffer[index] == '\n') {
        return false;
    }

    /* Otherwise, continue reading */
    return true;
}

/**
 *@brief Trim whitespace from a string
 * This function removes leading and trailing whitespace characters from a string.
 * @param str The string to trim
 * @return char* A pointer to the trimmed string
 */
char *trim_whitespace(char *str) {

    char *ptr_leading;
    char *ptr_trailing;

    if (str == NULL) return NULL;

    ptr_leading = str;
    ptr_trailing = str + strlen(str) - 1;

    /* As long is the first character is a space, move forward */
    while (isspace(*ptr_leading))
        ptr_leading++;

    /* At this point, ptr points to the first non space character in the string
         If that character is the end of the string, return it.
    */

    if (*ptr_leading == '\0') return ptr_leading;

    /* Now we trim the trailing whitespace
         The last character in the array besides the null terminator is located at index 'strlen(str) -2'
    */

    while (isspace(*ptr_trailing)) {
        *ptr_trailing = '\0';
        ptr_trailing--;
    }

    return ptr_leading;

}

/**
 *@brief Skip the .entry or .extern directive in an instruction
 * This function skips the .entry or .extern directive in an instruction by offsetting the buffer pointer.
 * @param _buffer A pointer to the buffer containing the instruction
 * @return char* A pointer to the buffer after skipping the directive
 */
char *skip_ent_or_ext(char *_buffer) {
    size_t i;
    char *ptr = _buffer;

    if (_buffer == NULL) {
        printf("Trying to skip '.entry' on a null buffer.\n");
        return NULL;
    }

    if (strstr(_buffer, ".entry") == NULL && strstr(_buffer, ".extern") == NULL) {
        return _buffer;
    }

    if (strncmp(_buffer, ".entry", 6) != 0 && strncmp(_buffer, ".extern", 7) != 0) {
        printf(" '.entry' or '.extern' directive must be at the beginning of the line.\n");
        return NULL;
    }

    /* Skip the directive */
    for (i = 0; i < strlen(_buffer) && !isspace(_buffer[i]); i++) {
        ptr++;
    }

    trim_whitespace(ptr);
    return ptr;
}

/**
 *@brief Skip the label name in an instruction
 * This function skips the label name in an instruction by offsetting the buffer pointer.
 * @param state The syntax state of the current instruction as defined in ds.h
 * @param _label_table A pointer to the label table
 */
void skip_label_name(syntax_state *state, label_table *_label_table) {

    int i;
    int offset = 0;
    int current_line = state->line_number;


    /** Find the first letter after the label name */
    for (i = 0; i < _label_table->size; i++) {
        if (_label_table->labels[i]->instruction_line == current_line &&
            (0 == strncmp(state->buffer_without_offset, _label_table->labels[i]->name, strlen(_label_table->labels[i]->name)))) {
            state->label_name_detected = true;
            state->label_key = _label_table->labels[i]->key;
            offset = 1 + strlen(_label_table->labels[i]->name);  /** another 1 for ':' */
            break;  /** Exit loop once the label is found */
        }
    }

    /* In a case where a label name is present but is invalid:
     * We'd like to notify the user about any syntax errors in the instruction defined by the label.
     * We check if the buffer contains a ':' character, then we skip the label name, even if it's invalid.
    */
    if (state->label_name_detected == false) {

        if (strstr(state->buffer_without_offset, ":") != NULL) {
            offset = 1 + strcspn(state->buffer_without_offset, ":");
        }
    }

    /** Point to the new "array" */
    state->buffer = &state->buffer[offset];
}

/**
 *@brief Check if a line is empty
 * This function checks if a line is empty by checking if it contains only whitespace characters.
 * @param str The line to check
 * @return bool The status of the check
 */
bool is_empty_line(char *str) {
    /* Remove the newline character at the end of the line, if there is one */
    str[strcspn(str, "\n")] = '\0';

    /* Check if the line is empty */
    if (strlen(str) == 0) {
        return true; /* Line is empty */
    }

    return false; /* Line is not empty */
}



/**
 *@brief This function duplicates a string by allocating memory for a new string and copying the contents of the original string.
 * We use this function to create a copy of a string that we can modify without affecting the original string.
 *
 * @param s The string to duplicate
 * @return char* The duplicated string
 */
char *my_strdup(char *s) {
    char *d = NULL;
    if (s == NULL) return NULL;
    d = (char *)calloc(strlen(s) + PADDING, sizeof(char));
    if (d == NULL) return NULL;
    strcpy(d, s);
    d[strlen(s)] = '\0';
    return d;
}
