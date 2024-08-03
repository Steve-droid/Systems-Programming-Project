#include "text_util.h"

/* Syntax State Utility */
syntax_state *create_syntax_state(void) {
    char *buffer = NULL;
    syntax_state *state = (syntax_state *)malloc(sizeof(syntax_state));

    if (state == NULL)
        return NULL;

    buffer = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if (buffer == NULL) {
        free(state);
        state = NULL;
        return NULL;
    }
    state->buffer_without_offset = buffer;
    state->index = -1;
    state->am_filename = NULL;
    state->as_filename = NULL;
    state->continue_reading = FALSE;
    state->_validation_state = invalid;
    state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
    state->buffer = buffer;
    state->line_number = -1;
    state->_inst = NULL;
    state->label_name = FALSE;
    state->label_key = -1;
    state->comma = FALSE;
    state->whitespace = FALSE;
    state->null_terminator = FALSE;
    state->new_line = FALSE;
    state->minus_sign = FALSE;
    state->plus_sign = FALSE;
    state->end_of_argument_by_space = FALSE;
    state->end_of_argument = FALSE;
    state->end_of_string = FALSE;
    state->first_quatiotion_mark = FALSE;
    state->last_quatiotion_mark = FALSE;
    state->digit = FALSE;
    state->is_data = FALSE;
    state->is_string = FALSE;
    state->is_entry = FALSE;
    state->is_extern = FALSE;
    return state;
}

void reset_syntax_state(syntax_state *state) {
    size_t i = 0;
    for (i = 0;i < MAX_LINE_LENGTH;i++) {
        state->buffer[i] = '\0';
    }

    state->buffer_without_offset = NULL;
    state->index = -1;

    state->_validation_state = invalid;
    state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
    state->_inst = NULL;
    state->continue_reading = FALSE;
    state->label_name = FALSE;
    state->label_key = -1;
    state->comma = FALSE;
    state->whitespace = FALSE;
    state->null_terminator = FALSE;
    state->new_line = FALSE;
    state->minus_sign = FALSE;
    state->plus_sign = FALSE;
    state->end_of_argument_by_space = FALSE;
    state->end_of_argument = FALSE;
    state->end_of_string = FALSE;
    state->first_quatiotion_mark = FALSE;
    state->last_quatiotion_mark = FALSE;
    state->digit = FALSE;
    state->is_data = FALSE;
    state->is_string = FALSE;
    state->is_entry = FALSE;
    state->is_extern = FALSE;
}

void initialize_command(syntax_state *data) {
    data->is_data = FALSE;
    data->is_string = FALSE;
    data->is_entry = FALSE;
    data->is_extern = FALSE;
}

void destroy_syntax_state(syntax_state **state) {
    if (state == NULL || (*state) == NULL)
        return;

    if ((*state)->buffer) {
        free((*state)->buffer);
        (*state)->buffer = NULL;
    }

    free((*state)->_inst);

    free((*state));
    (*state) = NULL;
}

void update_command(syntax_state *state, keyword *keyword_table, int command_key) {
    if (!strcmp(keyword_table[command_key].name, ".data")) {
        state->is_data = TRUE;
    }
    else if (!strcmp(keyword_table[command_key].name, ".string")) {
        state->is_string = TRUE;
    }
    else if (!strcmp(keyword_table[command_key].name, ".entry")) {
        state->is_entry = TRUE;
    }
    else if (!strcmp(keyword_table[command_key].name, ".extern")) {
        state->is_extern = TRUE;
    }
}

int continue_reading(char *instruction_buffer, syntax_state *state) {
    size_t index = state->index;
    size_t instruction_length = strlen(instruction_buffer);
    if (index >= instruction_length) {
        return FALSE;
    }

    /* If we reached a null terminator or a new line, break out of the loop */
    if (instruction_buffer[index] == '\0' || instruction_buffer[index] == '\n') {
        return FALSE;
    }

    /* Otherwise, continue reading */
    return TRUE;
}

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

char *skip_ent_or_ext(char *_buffer) {
    size_t i;
    char *ptr = _buffer;

    if (_buffer == NULL) {
        printf("*** ERROR ***\nTrying to skip '.entry' on a null buffer.\n");
        return NULL;
    }

    if (strstr(_buffer, ".entry") == NULL && strstr(_buffer, ".extern") == NULL) {
        return _buffer;
    }

    if (strncmp(_buffer, ".entry", 6) != 0 && strncmp(_buffer, ".extern", 7) != 0) {
        printf("*** ERROR ***\n '.entry' or '.extern' directive must be at the beginning of the line.\n");
        return NULL;
    }

    /* Skip the directive */
    for (i = 0; i < strlen(_buffer) && !isspace(_buffer[i]); i++) {
        ptr++;
    }

    trim_whitespace(ptr);
    return ptr;
}

void skip_label_name(syntax_state *state, label_table *_label_table) {

    int i;
    int offset = 0;
    int current_line = state->line_number;
    char *line = state->buffer;

    if (strncmp(line, ".entry", 6) == 0) {
        state->extern_or_entry = CONTAINS_ENTRY;
        state->is_entry = TRUE;
    }

    if (strncmp(line, ".extern", 7) == 0) {
        state->extern_or_entry = CONTAINS_EXTERN;
        state->is_extern = TRUE;
    }

    /** Find the first letter after the label name */
    for (i = 0; i < _label_table->size; i++) {
        if (_label_table->labels[i]->instruction_line == current_line &&
           ( 0 == strncmp( state->buffer_without_offset , _label_table->labels[i]->name, strlen(_label_table->labels[i]->name)))) {
                state->label_name = TRUE;
                state->label_key = _label_table->labels[i]->key;
                offset = 1 + strlen(_label_table->labels[i]->name);  /** another 1 for ':' */
                break;  /** Exit loop once the label is found */
        }
    }

    /** Point to the new "array" */
    state->buffer = &state->buffer[offset];
}

int binary_array_to_int(int *array) {
    int sign_bit, value, i;
    sign_bit = array[0];
    value = 0;

    /** Iterate over the array to construct the integer value */
    for (i = 0; i < OUTPUT_COMMAND_LEN; i++) {
        value = (value << 1) | array[i];
    }

    /** If the sign bit is 1, the number is negative in two's complement */
    if (sign_bit == 1) {
        /** Subtract 2^15 to convert to negative number */
        value -= (1 << OUTPUT_COMMAND_LEN);
    }

    return value;
}

int *convert_twodim_array_to_onedim(int **two_dim_array) {
    int elements = 0;
    int row = 0;
    int column = 0;
    int index = 0;
    int *one_dim_array = NULL;


    if (two_dim_array == NULL) {
        return NULL;
    }
    /* Calculate the total number of elements (excluding FLAG terminators) */
    for (row = 0; two_dim_array[row] != NULL; row++) {
        for (column = 0; two_dim_array[row][column] != FLAG; column++) {
            elements++;
        }
    }

    /* Allocate memory for the 1D array plus one additional space for the FLAG */
    one_dim_array = (int *)malloc((elements + 1) * sizeof(int));
    if (one_dim_array == NULL) {
        printf("*** ERROR ***\nMemory allocation failed\n");
        return NULL;
    }

    /* Copy elements from the 2D array to the 1D array */
    for (row = 0; two_dim_array[row] != NULL; row++) {
        for (column = 0; two_dim_array[row][column] != FLAG; column++) {
            one_dim_array[index++] = two_dim_array[row][column];
        }
    }

    /* Add the FLAG as the terminator of the 1D array */
    one_dim_array[index] = FLAG;

    return one_dim_array;
}

void initialize_int_array(int *arr, int size) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = 0;
    }
}

void print_array_in_binary(int *arr) {
    int i;
    if (arr == NULL) {
        return;
    }

    for (i = 0; arr[i] != FLAG; i++) {
        print_binary_2(arr[i]);
    }

    printf("\n");
}

int is_empty_line(char *str) {
    /* Remove the newline character at the end of the line, if there is one */
    str[strcspn(str, "\n")] = '\0';

    /* Check if the line is empty */
    if (strlen(str) == 0) {
        return TRUE; /* Line is empty */
    }

    return FALSE; /* Line is not empty */
}

int char_to_int(char c) {
    if (isdigit(c)) {
        return c - '0';
    }
    if (c == '+') {
        return PLUS;
    }
    if (c == '-') {
        return MINUS;
    }
    else if (c == ',') {
        return  COMMA;
    }
    else {
        printf("*** ERROR ***\nThe character is not a digit.\n");
        return -1; /* Return an error code if the character is not a digit */
    }
}

int *convert_to_int_array(char *str) {
    /* Temporary variables */
    int *result = NULL;
    char *token = NULL;
    int index = 0;

    /* Tokenize the string and convert each token to an integer */
    token = strtok(str, ",");
    while (token != NULL) {
        /* Reallocate memory for the integer array */
        result = (int *)realloc(result, (index + 1) * sizeof(int));
        if (result == NULL) {
            printf("*** ERROR ***\nMemory allocation failed\n");
            exit(1);
        }

        /* Convert token to integer and store in the array */
        result[index++] = atoi(token);

        /* Get next token */
        token = strtok(NULL, ",");
    }

    /* Reallocate memory to add the FLAG at the end */
    result = (int *)realloc(result, (index + 1) * sizeof(int));
    if (result == NULL) {
        printf("*** ERROR ***\nMemory allocation failed\n");
        exit(1);
    }

    /* Set the FLAG at the end of the array */
    result[index] = FLAG;

    return result;
}

/*Print utils*/
void print_2D_array(int **arr) {
    int i, j, counter;
    counter = 99;

    printf("THE 2D ARRAY:\n");
    for (i = 0; arr[i] != NULL; i++) {
        for (j = 0; arr[i][j] != FLAG; j++) {
            counter++;
            printf("line: %d, number: %d\n", counter, arr[i][j]);
        }
    }
    printf("END 2D ARRAY\n");
}

void print_bits(unsigned value, int num_bits) {
    int i;
    for (i = num_bits - 1; i >= 0; i--) {
        printf("%u", (value >> i) & 1);
    }
}

void print_binary_to_file(uint16_t word, FILE *file_ptr) {
    int i;
    fprintf(file_ptr, "\t\t\t");
    for (i = 14; i >= 0; i--) {
        fprintf(file_ptr, "%u", (word >> i) & 1);

    }
    fprintf(file_ptr, "\n");
}


void print_binary_2(int num) {
    int i;
    unsigned int mask;
    unsigned int u_num;
    char binary[16];
    binary[15] = '\0';  /* Null terminator */

    mask = 1 << 14;  /* Mask for the 15th bit */

    /** If the number is negative, convert it to 2's complement representation */
    if (num < 0) {
        u_num = (unsigned int)(num + (1 << 15));  /* 2's complement for negative number */
    }
    else {
        u_num = (unsigned int)num;
    }
    /** Check if the number is greater than or equal to FIRST_KEY and print it if so */

    /** Create the binary string */
    for (i = 0; i < 15; i++) {
        binary[i] = (u_num & mask) ? '1' : '0';
        mask >>= 1;
    }

    /** Print the resulting binary string */
    printf("%s\n", binary);
}


char *my_strdup(char *s) {
    char *d = NULL;
    if (s == NULL) return NULL;
    d = calloc(strlen(s) + 4, sizeof(char));
    if (d == NULL) return NULL;
    strcpy(d, s);
    d[strlen(s)] = '\0';
    return d;
}
