#include "text_util.h"

/* Syntax State Utility */
syntax_state *create_syntax_state() {
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

    state->index = -1;
    state->continue_reading = false;
    state->_validation_state = invalid;
    state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
    state->buffer = buffer;
    state->line_number = -1;
    state->_inst = NULL;
    state->label_name = false;
    state->label_key = -1;
    state->comma = false;
    state->whitespace = false;
    state->null_terminator = false;
    state->new_line = false;
    state->minus_sign = false;
    state->plus_sign = false;
    state->end_of_argument_by_space = false;
    state->end_of_argument = false;
    state->end_of_string = false;
    state->first_quatiotion_mark = false;
    state->last_quatiotion_mark = false;
    state->digit = false;
    state->is_data = false;
    state->is_string = false;
    state->is_entry = false;
    state->is_extern = false;
    return state;
}

void reset_syntax_state(syntax_state *state) {
    size_t i = 0;
    for (i = 0;i < MAX_LINE_LENGTH;i++) {
        state->buffer[i] = '\0';
    }
    state->index = -1;
    state->_validation_state = invalid;
    state->extern_or_entry = NEITHER_EXTERN_NOR_ENTRY;
    state->_inst = NULL;
    state->continue_reading = false;
    state->label_name = false;
    state->label_key = -1;
    state->comma = false;
    state->whitespace = false;
    state->null_terminator = false;
    state->new_line = false;
    state->minus_sign = false;
    state->plus_sign = false;
    state->end_of_argument_by_space = false;
    state->end_of_argument = false;
    state->end_of_string = false;
    state->first_quatiotion_mark = false;
    state->last_quatiotion_mark = false;
    state->digit = false;
    state->is_data = false;
    state->is_string = false;
    state->is_entry = false;
    state->is_extern = false;
}

void initialize_command(syntax_state *data) {
    data->is_data = false;
    data->is_string = false;
    data->is_entry = false;
    data->is_extern = false;
}

void destroy_syntax_state(syntax_state **state) {
    if (state == NULL || (*state) == NULL)
        return;

    if ((*state)->buffer) {
        free((*state)->buffer);
        (*state)->buffer = NULL;
    }

    free((*state));
    (*state) = NULL;
}

void update_command(syntax_state *state, keyword *keyword_table, int command_key) {
    if (!strcmp(keyword_table[command_key].name, ".data")) {
        state->is_data = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".string")) {
        state->is_string = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".entry")) {
        state->is_entry = true;
    }
    else if (!strcmp(keyword_table[command_key].name, ".extern")) {
        state->is_extern = true;
    }
}

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
        printf("ERROR- Trying to skip '.entry' on a null buffer.\n");
        return NULL;
    }

    if (strstr(_buffer, ".entry") == NULL && strstr(_buffer, ".extern") == NULL) {
        return _buffer;
    }

    if (strncmp(_buffer, ".entry", 6) != 0 && strncmp(_buffer, ".extern", 7) != 0) {
        printf("Error- '.entry' or '.extern' directive must be at the beginning of the line.\n");
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
        state->is_entry = true;
        return;
    }

    if (strncmp(line, ".extern", 7) == 0) {
        state->extern_or_entry = CONTAINS_EXTERN;
        state->is_extern = true;
        return;
    }

    /** Find the first letter after the label name */
    for (i = 0; i < _label_table->size; i++) {
        if (_label_table->labels[i]->instruction_line == current_line) {
            state->label_name = true;
            state->label_key = _label_table->labels[i]->key;
            offset = 1 + strlen(_label_table->labels[i]->name);  /** another 1 for ':' */
            break;  /** Exit loop once the label is found */
        }
    }

    /** Point to the new "array" */
    state->buffer += offset;
}

void int_to_binary_array(int num, bin_word *binary_word, int start, int finish) {
    size_t i;

    /** If the number is negative, adjust for two's complement */
    if (num < 0) {
        num += (1 << (finish - start + 1));
    }
    /** Convert the number to binary and store it in the array */
    for (i = finish; i >= start; i--) {
        binary_word->bits_vec[i] = num % 2;
        num /= 2;
    }
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
        printf("ERROR-Memory allocation failed\n");
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
        print_binary(arr[i]);
    }

    printf("\n");
}



bool is_empty_line(char *str) {
    /* Remove the newline character at the end of the line, if there is one */
    str[strcspn(str, "\n")] = '\0';

    /* Check if the line is empty */
    if (strlen(str) == 0) {
        return true; /* Line is empty */
    }

    return false; /* Line is not empty */
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
        printf("Error: The character is not a digit.\n");
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
            printf("Memory allocation failed\n");
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
        printf("Memory allocation failed\n");
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

void print_binary(int num) {
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

    if (num >= FIRST_KEY) {
        printf("%d\n", num);
        return;
    }

    /** Create the binary string */
    for (i = 0; i < 15; i++) {
        binary[i] = (u_num & mask) ? '1' : '0';
        mask >>= 1;
    }

    /** Print the resulting binary string */
    printf("%s\n", binary);
}








































