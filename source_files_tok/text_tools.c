#include "text_tools.h"

char *trim_whitespace(char *str) {

    char *ptr_leading;
    char *ptr_trailing;

    if (str == NULL || *str == '\0') return NULL;

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


char *skip_label_name(buffer_data *_buffer_data, label_table *_label_table, status *_entry_or_external) {
    int i;
    int offset = 0;
    int current_line = _buffer_data->line_counter;
    char *line = _buffer_data->buffer;

    if (strncmp(line, ".entry", 6) == 0) {
        *_entry_or_external = CONTAINS_ENTRY;
        return line;
    }

    if (strncmp(line, ".extern", 7) == 0) {
        *_entry_or_external = CONTAINS_EXTERN;
        return line;
    }

    /** Find the first letter after the label name */
    for (i = 0; i < _label_table->size; i++) {
        if (_label_table->labels[i]->instruction_line == current_line) {
            offset = 1 + strlen(_label_table->labels[i]->name);  /** another 1 for ':' */
            break;  /** Exit loop once the label is found */
        }
    }

    /** Point to the new "array" */
    return line + offset;
}


void str_cpy_until_char(char *destination, const char *source, char x) {
    int i;

    for (i = 0; !(source[i] == '\0' || source[i] == x); i++) {
        destination[i] = source[i];
    }
    destination[i] = '\0';
}

void initialize_char_array(char *char_array) {
    int i;
    int array_len = strlen(char_array);

    for (i = 0; i < array_len; i++) {
        char_array[i] = '\0';
    }
}
