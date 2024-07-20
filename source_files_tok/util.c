#include "util.h"

/*---------------File Utilities---------------*/
char *create_file_name(char *initial_filename, char *extension) {
    size_t initial_name_len = strlen(initial_filename);
    size_t extension_len = strlen(extension);
    size_t new_filename_len = initial_name_len + extension_len; /*Length of the new filename*/
    char *new_filename = NULL;

    /* Allocate memory for the new filename */
    new_filename = (char *)malloc((new_filename_len + 1));
    if (new_filename == NULL) err(errno, "Memory allocation failed\n");

    /* Concatenate the initial name and the extension */
    strcpy(new_filename, initial_filename);
    strcat(new_filename, extension);

    return new_filename;
}

status remove_file_extension(char *full_filename, char **generic_filename) {

    size_t full_filename_length = 0;
    char *extension = NULL;

    if (full_filename == NULL) {
        printf("Trying to remove extention from an empty filename\nExiting...\n");
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    full_filename_length = strlen(full_filename);

    *(generic_filename) = (char *)malloc((full_filename_length + 1) * sizeof(char));

    if (*(generic_filename) == NULL) {
        printf("Memory allocation error while creating generic filename for %s\nExiting...\n", full_filename);
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    strcpy(*(generic_filename), full_filename);
    extension = strstr(*(generic_filename), ".");


    if (extension == NULL) {
        printf("Trying to remove extention from a filename with no extention\nExiting...\n");
        free(extension);
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    *extension = '\0'; /*Null terminate the generic filename*/
    return STATUS_OK;
}

status copy_file_contents(char *src_filename, char *dest_filename) {
    FILE *src_file = fopen(src_filename, "r");
    FILE *dest_file = fopen(dest_filename, "w");
    char buffer[BUFSIZ] = { 0 };
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    bool write_success = true;


    if (src_file == NULL) {
        fprintf(stderr, "Failed to open source file %s\n", src_filename);
        return STATUS_ERROR_OPEN_SRC;
    }

    if (dest_file == NULL) {
        fprintf(stderr, "Failed to open destination file %s\n", dest_filename);
        fclose(src_file);
        return STATUS_ERROR_OPEN_DEST;
    }

    while ((bytes_read = fread(buffer, sizeof(char), BUFSIZ, src_file))) {
        /* Write the bytes you read into the destination file */
        bytes_written = fwrite(buffer, sizeof(char), bytes_read, dest_file);

        /* If we failed to write exactly what we read, data was lost in the process */
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Error writing to file %s\n", dest_filename);
            write_success = false;
            break;
        }
    }

    fclose(src_file);
    fclose(dest_file);

    /* If the operation resulted in a partial copy, we remove the destination file */
    if (write_success == false) {
        remove(dest_filename);
        return STATUS_ERROR_WRITE;
    }

    return STATUS_OK;
}

status remove_whitespace(char *filename) {
    FILE *file;
    FILE *tmp_file;
    char *start;
    char *end;
    char *tmp;
    size_t i;
    char tmp_filename[] = "tmpfileXXXXXX";
    int temp_file_descriptor;
    char line[BUFSIZ];
    int original_line_count = 0;
    int cleaned_line_count = 0;
    bool line_contains_only_whitespace = false;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return STATUS_ERROR_OPEN_SRC;
    }

    /* Create a temporary file to write the cleaned lines */
    temp_file_descriptor = mkstemp(tmp_filename);
    if (temp_file_descriptor == -1) {
        fprintf(stderr, "Failed to create temporary file\n");
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }
    tmp_file = fdopen(temp_file_descriptor, "w");

    if (tmp_file == NULL) {
        fprintf(stderr, "Failed to open temporary file\n");
        close(temp_file_descriptor);
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }

    /* Count lines in the original file and write cleaned lines to the temporary file */
    while (fgets(line, sizeof(line), file)) {

        start = line;
        end = start + strlen(start) - 1;
        line_contains_only_whitespace = true;

        /*Check if the line contains only whitespace. If so, skip the line*/
        for (i = 0, tmp = start;line_contains_only_whitespace == true && i < strlen(start) && tmp != NULL;i++, tmp++) {
            if (!isspace(*tmp)) {
                line_contains_only_whitespace = false;
                break;
            }
        }

        if (line_contains_only_whitespace) continue;



        original_line_count++;

        /* Remove leading whitespace */
        while (isspace((unsigned char)*start)) {
            start++;
        }

        /* Remove trailing whitespace */
        while (end > start && isspace((unsigned char)*end)) {
            end--;
        }
        *(end + 1) = '\0';  /* Null-terminate the string*/

        /* Only write non-empty lines to the temporary file */
        if (*start != '\0') {
            if (fprintf(tmp_file, "%s\n", start) < 0) {
                fprintf(stderr, "Error writing to temporary file\n");
                fclose(file);
                fclose(tmp_file);
                remove(tmp_filename);
                return STATUS_ERROR_WRITE;
            }
            cleaned_line_count++;
        }
    }

    fclose(file);
    fclose(tmp_file);

    /* Verify that the number of lines matches */
    if (original_line_count != cleaned_line_count) {
        fprintf(stderr, "Line count mismatch: original=%d, cleaned=%d\n", original_line_count, cleaned_line_count);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    /* Replace the original file with the cleaned temporary file */
    if (remove(filename) != 0) {
        fprintf(stderr, "Failed to remove original file %s: %s\n", filename, strerror(errno));
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }
    if (rename(tmp_filename, filename) != 0) {
        fprintf(stderr, "Failed to rename temporary file to %s: %s\n", filename, strerror(errno));
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    return STATUS_OK;
}

status duplicate_files(char ***backup_filenames, int file_count, char *filenames[], char *extention) {
    char *current_filename = filenames[0];
    char *filename_copy = NULL;
    int i;
    int backup_filenames_count = 0;
    int current_filename_len = 0;
    int filename_copy_len = 0;

    if (filenames == NULL || file_count < 1) {
        printf("Attempting to backup an empty file list. Exiting...");
        exit(EXIT_FAILURE);
    }

    for (i = 0;i < file_count;i++) {
        current_filename = filenames[i];

        if (current_filename == NULL) {
            printf("Error while backing up argument names. Original filename is NULL. Exiting...");
            exit(EXIT_FAILURE);
        }

        current_filename_len = strlen(current_filename);
        filename_copy_len = current_filename_len + strlen(extention) - 1;
        filename_copy = (char *)malloc(filename_copy_len);
        if (filename_copy == NULL) {
            printf("Memory allocation error while creating a backup %s filename", extention);
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
        strcpy(filename_copy, current_filename);
        strcat(filename_copy, extention);

        printf("Copying %s file into %s%s... ", current_filename, current_filename, extention);
        if (filename_copy == NULL) {
            printf("Error while backing up files. Removing created backups...\n");
            for (i = 0;i < backup_filenames_count;i++) {
                if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
            }
            printf("Done. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        printf("Done\n");

        if (copy_file_contents(current_filename, filename_copy) != STATUS_OK) {
            printf("Error while creating a backup file for %s. Exiting...\n", current_filename);
            return STATUS_ERROR_WHILE_CREATING_FILENAME;
        }

        *backup_filenames[i] = filename_copy;
        backup_filenames_count++;
    }

    if (backup_filenames_count != file_count) {
        printf("Error while backing up files. Number of files in backup is different from the original number of files. ");
        printf("\nRemoving backup filenames...");
        for (i = 0;i < backup_filenames_count;i++) {
            if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
        }
        printf("Done.\nExiting...\n");
        exit(EXIT_FAILURE);
    }

    return STATUS_OK;
}

status delete_filenames(size_t file_amount, char **filenames) {
    size_t i;
    if (filenames == NULL) {
        printf("ERROR: Attempted to free filenames but filename array is empty. Exiting...");
        return STATUS_ERROR;
    }

    printf("Deleting filenames...\n");
    for (i = 0;i < file_amount;i++) {

        if (filenames[i] == NULL) {
            printf("Error: failed to delete filename number %lu", i);
            printf("Filename deleted by another part of the program.\n");
            printf("Exiting...\n");
            return STATUS_ERROR;
        }

        printf("Deleting filename %s... ", filenames[i]);
        free(filenames[i]);
        printf("Done\n");
    }

    return STATUS_OK;
}

/*---------------Other Utilities---------------*/




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







int command_number_by_key(keyword *keyword_table, int key) {
    int i, flag;

    flag = UNDEFINED;

    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (key == keyword_table[i].key) {
            flag = i;
        }
    }
    return flag;
}

/**
 *@brief
 * @param pre_decoded- array of strings representing a single pre-decoded instruction
 * @param pre_decoded_size
 */
void print_instruction(inst *_inst) {
    size_t i;
    size_t size = 0;
    static size_t inst_index = 0;
    if (_inst->num_tokens == UNKNOWN_NUMBER_OF_ARGUMENTS) {
        size = 2;
    }

    size = _inst->num_tokens;
    printf("\n-----------------------------------\n");
    printf("Instruction %lu: ", inst_index);
    for (i = 0; i < size; i++) {
        printf("%s ", _inst->tokens[i]);
    }
    printf("\n-----------------------------------\n");

    inst_index++;
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

/**
 *@brief Convert a binary array to an integer
 *
 * @param array
 * @return int
 */
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

/**
 *@brief Get a 2D array of pointers to arrays of integers and convert it to a 1D array
 * Each cell in the 2D array is a pointer to an array of integers
 * This function converts the 2D array to a 1D array by copying the integers from the 2D array to the 1D array
 * @param array2D
 * @return int*
 */
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

/**
 *@brief Get an array of integers and print them in binary
 *
 * @param arr
 */
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
    else if (c == '+') {
        return PLUS;
    }
    else if (c == '-') {
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

/**
 *@brief
 * Get a string of numbers seperated by commas
 * Create an array of integers from the string
 * The array contains the numbers in the string
 * @param str The string containing the numbers
 * @return int* The array of integers
 */
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

/**
 *@brief Print a 2D array
 * Print a 2D array of integers
 * @param arr The 2D array to print
 */
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



void init_buffer_data(buffer_data *data) {
    size_t i;


    /* If the buffer is allocated, initialize it */
    if (data->buffer != NULL) {
        for (i = 0; i < MAX_LINE_LENGTH; i++) {
            data->buffer[i] = '\0';
        }
    }

    /* Initialize the buffer data */
    data->index = 0;
    data->line_counter = -1;
    data->command_key = UNDEFINED;
}

void reset_buffer(char *buffer) {
    size_t i;
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        buffer[i] = '\0';
    }
}

status create_buffer_data(buffer_data **data) {


    (*data) = (buffer_data *)malloc(sizeof(buffer_data));
    if (data == NULL) {
        printf("ERROR- ALLOCATION FAILED");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    (*data)->command_key = UNDEFINED;
    (*data)->index = 0;
    (*data)->line_counter = -1;
    (*data)->command_key = UNDEFINED;
    (*data)->buffer = NULL;

    return STATUS_OK;
}


void destroy_buffer_data(buffer_data *data) {
    if (data->buffer != NULL) {
        free(data->buffer);
    }
    free(data);
}

