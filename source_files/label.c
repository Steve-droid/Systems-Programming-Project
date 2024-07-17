#include "label.h"


/**
 *@brief Fill the label table with labels from the .am file.
 *
 * @param am_filename The name of the .am file to read.
 * @param m_table The macro table to check for macro names.
 * @param keywords_table The keywords table to check for keyword names.
 * @return label_table*
 */
label_table *fill_label_table(char *am_filename, macro_table *m_table, keyword *keywords_table) {
    char instruction_buffer[MAX_LINE_LENGTH] = { '\0' }; /* Buffer to hold each line */
    char *label_name = NULL;
    label_table *_label_table = NULL;
    size_t lines_in_file = 0;
    FILE *am_file = NULL;
    label *new_label = NULL;

    /* Open the .am file for reading lines */
    am_file = fopen(am_filename, "r");
    if (am_file == NULL) {
        printf("Error- Failed to open the file '%s'. Exiting...\n", am_filename);
        return NULL;
    }

    /* Print the current .am filename */
    printf("%s\n", am_filename);

    /* Initialize the label table */
    _label_table = new_empty_label_table(&_label_table);
    if (_label_table == NULL) {
        printf("ERROR- Failed to allocate memory for label table.\nFile name: '%s'.\n Exiting...\n", am_filename);
        return NULL;
    }

    lines_in_file = -1; /* lines start from 0*/

    while (fgets(instruction_buffer, sizeof(instruction_buffer), am_file)) { /* Read every line from the .am file */
        label_name = NULL;

        /* Skip empty lines */
        if (is_empty_line(instruction_buffer)) {
            continue;
        }
        /* Skip comment lines */
        if (instruction_buffer[0] == ';') {
            continue;
        }

        lines_in_file++;
        remove_prefix_spaces(instruction_buffer);
        /* Check if the line contains a label definition. if it does, save the label name */
        label_name = extract_label_name_from_instruction(instruction_buffer);

        /* If the line does not contain a label definition, continue to the next line */
        if (label_name == NULL) {
            continue;
        }

        /* If the line contains a label definition, check if the label name is valid */
        if (label_name_is_valid(_label_table, label_name, keywords_table, m_table)) {

            new_label = new_empty_label(&new_label);

            /* Fill the label data and keep the address as UNDEFINED until the 2nd pass */
            label_update_fields(&new_label, label_name, lines_in_file, UNDEFINED);
            _label_table = insert_label(_label_table, new_label);

            if (_label_table == NULL) {
                printf("ERROR- Failed to insert label '%s' into label table.\nFile name: '%s'.\n Exiting...\n", label_name, am_filename);
                return NULL;
            }
        }
    }
    fclose(am_file);
    return _label_table;
}

/**
 *@brief Check if a label name is valid.
 * The label name must be:
 * 1. Not NULL.
 * 2. Not too long.
 * 3. Contain only alphabetical characters and digits.
 * 4. Not a keyword name.
 * 5. Not a macro name.
 * 6. Not already in the label table.
 * @param _label_table
 * @param label_name
 * @param keywords_table
 * @param _macro_table
 * @return true If the label name answers all the conditions.
 * @return false If the label name does not answer one of the conditions.
 */
bool label_name_is_valid(label_table *_label_table, char *label_name, keyword *keywords_table, macro_table *_macro_table) {
    int i;
    char *result = NULL;
    macro *result_macro = NULL;
    keyword *result_keyword = NULL;

    /* Check if the label name is NULL or too long */
    if (label_name == NULL) {
        printf("ERROR- Checking a NULL label name.\n");
        return false;
    }

    if (strlen(label_name) > MAX_LABEL_LENGTH) {
        printf("ERROR- Label name %s is too long. Max label length is %d.\n", label_name, MAX_LABEL_LENGTH);
        return false;
    }

    /*Check if the label name contains only alphabetical characters and digits */
    for (i = 0; label_name[i] != '\0'; i++) {
        if (!(isalpha(label_name[i]) || isdigit(label_name[i]))) {
            return false;
        }
    }

    /* Check if the label name is a keyword */
    result_keyword = get_keyword_by_name(keywords_table, label_name);
    if (result_keyword != NULL) {
        printf("ERROR- Label name %s cannot be the same as the %s keyword.\n", label_name, result);
        return false;
    }

    /* Check if the label name is a macro name */
    result_macro = get_macro(_macro_table, label_name);
    if (result_macro != NULL) {
        printf("ERROR- Label name '%s' cannot be the same as the '%s' macro name.\n", label_name, result);
        return false;
    }

    /* Check if the label name is already in the label table */
    if (get_label(_label_table, label_name) != NULL) {
        printf("ERROR- Label name '%s' is already in the label table.\n", label_name);
        return false;
    }

    /* If we reached this point, the label name is valid */
    return true;
}

/**
 *@brief Check if a given instruction contains a label definition.
 * If the instruction contains the ':' character, return the string up to the ':' character.
 * If the instruction does not contain the ':' character, return NULL.
 * @param instruction A string that represents an instruction.
 * @return char* A string that represents a possible label name if the instruction contains a label definition.
 * @return NULL If the instruction does not contain a label definition.
 */
char *extract_label_name_from_instruction(char *instruction) {
    char *label_name = NULL;
    char *instruction_copy = NULL;
    char *instruction_copy_ptr = NULL;
    char *instruction_copy_ptr2 = NULL;
    int label_name_len = 0;

    /* Allocate memory for a copy of the instruction */
    instruction_copy = (char *)malloc(strlen(instruction) * sizeof(char));
    if (instruction_copy == NULL) {
        printf("ERROR- Failed to allocate memory for instruction copy.\n");
        return NULL;
    }

    /* Copy the instruction to a new buffer */
    strcpy(instruction_copy, instruction);

    /* Find the ':' character in the instruction */
    instruction_copy_ptr = instruction_copy;
    instruction_copy_ptr2 = instruction_copy;

    while (*instruction_copy_ptr2 != '\0') {

        /* If the ':' character is found, return the string up to the ':' character */
        if (*instruction_copy_ptr2 == ':') {
            /* Null terminate the label name */
            *instruction_copy_ptr2 = '\0';
            label_name_len = strlen(instruction_copy_ptr) + 1;
            label_name = (char *)malloc(label_name_len * sizeof(char));
            if (label_name == NULL) {
                free(instruction_copy);
                return NULL;
            }

            /* Copy the label name to a new buffer */
            strncpy(label_name, instruction_copy_ptr, label_name_len);

            /* Free the instruction copy buffer */
            free(instruction_copy);

            /* Return the label name */
            return label_name;
        }

        /* If the ':' character is not found, continue to the next character */
        instruction_copy_ptr2++;
    }

    /* Free the instruction copy buffer */
    free(instruction_copy);

    /* If we reached this point, the instruction does not contain a label definition */
    return NULL;
}

/**
 *@brief Print the label table.
 * Print the label table to the console.
 * Parameters that are printed for each label: name, key, instruction line, size.
 * @param _label_table The label table to print.
 */
void print_label_table(label_table *_label_table) {
    int i;

    if (_label_table == NULL) {
        printf("ERROR- Trying to print a NULL label table.\n");
        return;
    }
    printf("PRINTING LABEL TABLE:\n");

    for (i = 0; i < _label_table->size; i++) {
        printf("name: %s \n", _label_table->labels[i]->name);
        printf("key: %lu \n", _label_table->labels[i]->key);
        printf("line: %lu \n", _label_table->labels[i]->instruction_line);
        printf("size: %lu \n\n", _label_table->labels[i]->size);
    }
}

/**
 *@brief Initialize a new label table.
 * Allocate memory for a new label table and initialize its fields.
 * @return label_table* A pointer to the new label table.
 * @return NULL If the memory allocation failed.
 */
label_table *new_empty_label_table(label_table **new_label_table) {
    if (!(*new_label_table = (label_table *)malloc(sizeof(label_table)))) {
        return NULL;
    }
    (*new_label_table)->size = 0;
    (*new_label_table)->capacity = 1;
    (*new_label_table)->labels = (label **)malloc((*new_label_table)->capacity * sizeof(label *));
    if ((*new_label_table)->labels == NULL) {
        free(*new_label_table);
        return NULL;
    }
    return *new_label_table;
}

/**
*@brief Initialize a new label.
* Allocate memory for a new label and initialize its fields.
* @return label* A pointer to the new label.
*/
label *new_empty_label(label **new_label) {
    if (!(*new_label = (label *)malloc(sizeof(label)))) {
        return NULL;
    }

    /* Initialize the label fields */
    (*new_label)->key = 0;
    (*new_label)->instruction_line = 0;
    (*new_label)->address = 0;
    (*new_label)->size = 0;
    (*new_label)->entry_or_extern = UNDEFINED;

    return *new_label;
}

/**
 *@brief Insert a new label into the label table.
 *
 * @param table The label table to insert the label into.
 * @param _lable The label to insert into the table.
 * @return label_table* If the label was successfully inserted into the table.
 * @return NULL If the memory allocation failed.
 */
label_table *insert_label(label_table *_label_table, label *_lable) {
    if (_label_table->size == _label_table->capacity) {
        _label_table->capacity *= 2;
        _label_table->labels = (label **)realloc(_label_table->labels, _label_table->capacity * sizeof(label *));
        if (_label_table->labels == NULL) {
            free(_label_table);
            return NULL;
        }
    }
    _label_table->labels[_label_table->size] = _lable;
    _label_table->size++;
    return _label_table;
}

/**
 *@brief Fill the label fields.
 * Fill the label name, key, instruction line, address, size and entry_or_extern fields.
 * @param new_label A pointer to the new label.
 * @param label_name The label name to fill.
 * @param line_counter The line number of the label definition.
 * @param address The address of the label definition.
 */
void label_update_fields(label **new_label, char *label_name, int line_counter, int address) {
    static int label_key = FIRST_KEY;
    /* Fill label name */
    strcpy((*new_label)->name, label_name);
    (*new_label)->key = label_key;
    label_key++;
    (*new_label)->instruction_line = line_counter;
    (*new_label)->address = address;
    (*new_label)->size = 0;
    (*new_label)->entry_or_extern = UNDEFINED;
}

/**
 *@brief Search for a label name in the label table.
 * If the label name is found in the table, return a pointer to the label.
 * If the label name is not found in the table, return NULL.
 * @param label_table The label table to search in.
 * @param label_name The label name to search for.
 * @return label*
 */
label *get_label(label_table *_label_table, char *label_name) {
    size_t i;

    for (i = 0; (i < _label_table->size) && (_label_table->labels[i] != NULL); i++) {
        if (!strcmp(_label_table->labels[i]->name, label_name)) {
            return _label_table->labels[i];
        }
    }
    return NULL;
}


/**
 *@brief Free the memory allocated for a label.
 * Free the memory allocated for the label and set the pointer to NULL.
 * @param _label A pointer to the label to free.
 */
void label_destroy(label **_label) {
    free(*_label);
    *_label = NULL;
}

/**
 *@brief Free the memory allocated for a label table.
 * Free the memory allocated for the label table and set the pointer to NULL.
 * @param _label_table A pointer to the label table to free.
 */
void label_table_destroy(label_table **_label_table) {
    size_t i;
    for (i = 0; i < (*_label_table)->size; i++) {
        label_destroy(&(*_label_table)->labels[i]);
    }
    free((*_label_table)->labels);
    free(*_label_table);
    *_label_table = NULL;
}

/*fill_label_table_addresses*/
void assign_label_addresses(int **decoded_table, label_table *_label_table) {
    int i, j, k, counter;

    for (k = 0; k < _label_table->size; k++) {
        counter = FIRST_ADDRESS;
        for (i = 0; decoded_table && decoded_table[i] != NULL && i < _label_table->labels[k]->instruction_line; i++) {
            for (j = 0; decoded_table[i][j] != FLAG; j++) {
                counter++;
            }
        }
        _label_table->labels[k]->address = counter;
        _label_table->labels[k]->address = _label_table->labels[k]->address << 3;
        _label_table->labels[k]->address += _label_table->labels[k]->entry_or_extern == EXTERN ? 1 : 2;
    }
}

/*decode_label_addresses*/
void replace_label_keys_with_addresses(int *decoded, label_table *_label_table) {
    int i, k;

    for (k = 0; k < _label_table->size; k++) {
        for (i = 0; decoded && decoded[i] != FLAG; i++) {
            if (_label_table->labels[k]->key == decoded[i]) {
                decoded[i] = _label_table->labels[k]->address;
            }
        }
    }
}





