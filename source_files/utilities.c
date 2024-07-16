#include "utilities.h"
#include "decode_to_int_array.h"
#include "decoding.h"
#include "label_table.h"
#include "decode_to_string_array.h"
#define ERR -1



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
void initialize_char_array(char *char_array) {
    int i, array_len;

    array_len = (int)strlen(char_array);

    for (i = 0; i < array_len; i++) {
        char_array[i] = ' ';
    }
}

void remove_prefix_spaces(char *line) {
    int i, counter;
    counter = 0;

    /** Count leading spaces */
    while (line[counter] != '\0' && isspace((unsigned char)line[counter])) {
        counter++;
    }

    /** Shift the rest of the line to the front */
    if (counter > 0) {
        for (i = 0; line[i + counter] != '\0'; i++) {
            line[i] = line[i + counter];
        }
        line[i] = '\0';  /** Null-terminate the shifted string */
    }
}

char *pointer_after_label(char *line, label *label_table, int current_line) {
    int i, first_letter = 0;

    /** Find the first letter after the label name */
    for (i = 0; i < label_table[0].size; i++) {
        if (label_table[i].line == current_line) {
            first_letter = 1 + strlen(label_table[i].name);  /** another 1 for ':' */
            break;  /** Exit loop once the label is found */
        }
    }

    /** Point to the new "array" */
    return line + first_letter;
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

int map_addressing_method(char *str, label *label_table) {
    int i;

    /* case 0 */
    if (str[0] == '#') {
        if (str[1] == '\0') {
            return UNDEFINED;
        }
        if (str[1] != '-' && str[1] != '+' && !isdigit(str[1])) {
            return UNDEFINED;
        }
        if (str[1] == '-' || str[1] == '+') {
            if (!isdigit(str[2])) {
                return UNDEFINED;
            }
        }
        for (i = 2; i < (int)strlen(str); i++) {
            if (!isdigit(str[i])) {
                return UNDEFINED;
            }
        }
        return 0;
    }

    /* case 1 */
    for (i = 0; i < label_table[0].size; i++) {
        if (!strcmp(label_table[i].name, str)) {
            return 1;
        }
    }

    /* case 2 */
    if (str[0] == '*') {
        if (str[1] == '\0' || str[1] != 'r') {
            return UNDEFINED;
        }
        if (str[2] == '\0' || str[2] < '0' || str[2] > '7') {
            return UNDEFINED;
        }
        if (str[3] != '\0') {
            return UNDEFINED;
        }
        return 2;
    }

    /* case 3 */

    if (str[0] == 'r') {
        if (str[1] == '\0' || str[1] < '0' || str[1] > '7') {
            return UNDEFINED;
        }
        if (str[2] != '\0') {
            return UNDEFINED;
        }
        return 3;
    }

    /* else */
    return UNDEFINED;
}

void print_pre_decoded(string *pre_decoded, int pre_decoded_size) {
    int i;

    if (pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS) {
        pre_decoded_size = 2;
    }

    for (i = 0; i < pre_decoded_size; i++) {
        if (i != pre_decoded_size - 1) {
            printf("%s___", pre_decoded[i].data);
        }
        else {
            printf("%s\n", pre_decoded[i].data);
        }
    }
}

void int_to_binary_array(int num, int *arr, int from_cell, int to_cell) {
    int i;
    /** If the number is negative, adjust for two's complement */
    if (num < 0) {
        num += (1 << (to_cell - from_cell + 1));
    }

    /** Convert the number to binary and store it in the array */
    for (i = to_cell; i >= from_cell; i--) {
        arr[i] = num % 2;
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

int *convert_to_1D(int **array2D) {
    int total_elements, i, j, index;
    int *array1D = NULL;


    if (array2D == NULL) {
        return NULL;
    }
    /* Calculate the total number of elements (excluding FLAGs) */
    total_elements = 0;
    for (i = 0; array2D[i] != NULL; i++) {
        for (j = 0; array2D[i][j] != FLAG; j++) {
            total_elements++;
        }
    }

    /* Allocate memory for the 1D array plus one additional space for the FLAG */
    array1D = (int *)malloc((total_elements + 1) * sizeof(int));
    if (array1D == NULL) {
        printf("ERROR-Memory allocation failed\n");
        return NULL;
    }

    /* Copy elements from the 2D array to the 1D array */
    index = 0;
    for (i = 0; array2D[i] != NULL; i++) {
        for (j = 0; array2D[i][j] != FLAG; j++) {
            array1D[index++] = array2D[i][j];
        }
    }

    /* Add the FLAG as the terminator of the 1D array */
    array1D[index] = FLAG;

    return array1D;
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

void print_label_table(label *label_table) {
    int i;

    printf("LABEL-TABLE:\n");
    if (label_table == NULL) {
        printf("Error NULL FILE");
        return;
    }

    for (i = 0; i < label_table[0].size; i++) {
        printf("name: %s \n", label_table[i].name);
        printf("key: %d \n", label_table[i].key);
        printf("line: %d \n", label_table[i].line);
        printf("size: %d \n\n", label_table[i].size);

    }
}

keyword *fill_keywords_table() {
    keyword *keywords_table = NULL;

    keywords_table = (keyword *)malloc(KEYWORD_TABLE_LENGTH * sizeof(keyword));
    if (keywords_table == NULL) {
        printf("ERROR-ALLOCATION FAILED\n");
        return NULL;
    }

    /* Register keywords */
    strcpy(keywords_table[0].name, "r0");
    keywords_table[0].key = R0;
    keywords_table[0].length = 2;
    strcpy(keywords_table[1].name, "r1");
    keywords_table[1].key = R1;
    keywords_table[1].length = 2;
    strcpy(keywords_table[2].name, "r2");
    keywords_table[2].key = R2;
    keywords_table[2].length = 2;
    strcpy(keywords_table[3].name, "r3");
    keywords_table[3].key = R3;
    keywords_table[3].length = 2;
    strcpy(keywords_table[4].name, "r4");
    keywords_table[4].key = R4;
    keywords_table[4].length = 2;
    strcpy(keywords_table[5].name, "r5");
    keywords_table[5].key = R5;
    keywords_table[5].length = 2;
    strcpy(keywords_table[6].name, "r6");
    keywords_table[6].key = R6;
    keywords_table[6].length = 2;
    strcpy(keywords_table[7].name, "r7");
    keywords_table[7].key = R7;
    keywords_table[7].length = 2;

    /* Operation keywords */
    strcpy(keywords_table[8].name, "mov");
    keywords_table[8].key = MOV;
    keywords_table[8].length = 3;
    strcpy(keywords_table[9].name, "cmp");
    keywords_table[9].key = CMP;
    keywords_table[9].length = 3;
    strcpy(keywords_table[10].name, "add");
    keywords_table[10].key = ADD;
    keywords_table[10].length = 3;
    strcpy(keywords_table[11].name, "sub");
    keywords_table[11].key = SUB;
    keywords_table[11].length = 3;
    strcpy(keywords_table[12].name, "lea");
    keywords_table[12].key = LEA;
    keywords_table[12].length = 3;
    strcpy(keywords_table[13].name, "clr");
    keywords_table[13].key = CLR;
    keywords_table[13].length = 3;
    strcpy(keywords_table[14].name, "not");
    keywords_table[14].key = NOT;
    keywords_table[14].length = 3;
    strcpy(keywords_table[15].name, "inc");
    keywords_table[15].key = INC;
    keywords_table[15].length = 3;
    strcpy(keywords_table[16].name, "dec");
    keywords_table[16].key = DEC;
    keywords_table[16].length = 3;
    strcpy(keywords_table[17].name, "jmp");
    keywords_table[17].key = JMP;
    keywords_table[17].length = 3;
    strcpy(keywords_table[18].name, "bne");
    keywords_table[18].key = BNE;
    keywords_table[18].length = 3;
    strcpy(keywords_table[19].name, "red");
    keywords_table[19].key = RED;
    keywords_table[19].length = 3;
    strcpy(keywords_table[20].name, "prn");
    keywords_table[20].key = PRN;
    keywords_table[20].length = 3;
    strcpy(keywords_table[21].name, "jsr");
    keywords_table[21].key = JSR;
    keywords_table[21].length = 3;
    strcpy(keywords_table[22].name, "rts");
    keywords_table[22].key = RTS;
    keywords_table[22].length = 3;
    strcpy(keywords_table[23].name, "stop");
    keywords_table[23].key = STOP;
    keywords_table[23].length = 4;

    /* Macro keywords */
    strcpy(keywords_table[24].name, "macr");
    keywords_table[24].key = MACR;
    keywords_table[24].length = 4;
    strcpy(keywords_table[25].name, "endmacr");
    keywords_table[25].key = ENDMACR;
    keywords_table[25].length = 7;

    /* Directive keywords */
    strcpy(keywords_table[26].name, ".data");
    keywords_table[26].key = DATA;
    keywords_table[26].length = 5;
    strcpy(keywords_table[27].name, ".string");
    keywords_table[27].key = STRING;
    keywords_table[27].length = 7;
    strcpy(keywords_table[28].name, ".entry");
    keywords_table[28].key = ENTRY;
    keywords_table[28].length = 6;
    strcpy(keywords_table[29].name, ".extern");
    keywords_table[29].key = EXTERN;
    keywords_table[29].length = 7;

    return keywords_table;
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

void remove_commas_from_str(char *str) {
    int i, j;

    for (i = 0, j = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        if (str[i] != ',') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void shift_left_str(char *str, int steps) {
    /* Get the length of the string */
    int length = strlen(str);
    int i;

    /* If steps are greater than or equal to string length, the result is an empty string */
    if (steps >= length) {
        str[0] = '\0';
        return;
    }

    /* Loop to shift the characters to the left by 'steps' positions */
    for (i = 0; i + steps < length; i++) {
        str[i] = str[i + steps];
    }

    /* Properly terminate the string */
    str[i] = '\0';
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
