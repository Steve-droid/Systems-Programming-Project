#include "symbols.h"
#define PADDING 2


keyword *get_keyword_by_name(keyword *keyword_table, char *name) {
    int i;
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (strcmp(keyword_table[i].name, name) == 0) {
            return &keyword_table[i];
        }
    }
    return NULL;
}

keyword *get_keyword_by_key(keyword *keyword_table, int key) {
    int i;
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (keyword_table[i].key == key) {
            return &keyword_table[i];
        }
    }
    return NULL;
}


keyword_name identify_command(syntax_state *state, label_table *_label_table, keyword *keyword_table) {
    int i;
    char *_instruction = NULL;
    char command_name[MAX_LINE_LENGTH] = { 0 };
    size_t buffer_length = strlen(state->buffer);
    bool command_found = false;


    if (state == NULL || _label_table == NULL || keyword_table == NULL) {
        printf("Tried to identify command with NULL arguments\n");
        return UNDEFINED_KEYWORD;
    }

    _instruction = state->buffer;

    /* Extract the command name from the line */
    for (i = 0; i < buffer_length && _instruction[i] != '\n' && !isspace(_instruction[i]); i++) {
        command_name[i] = _instruction[i];
    }

    /* Null-terminate the command name */
    command_name[i] = '\0';

    /* Compare the extracted command name against the keyword table */
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {

        /* Check if the command name is found in the keyword table */
        command_found = (strncmp(command_name, keyword_table[i].name, strlen(keyword_table[i].name)) == 0);

        if (command_found) {

            /* Check if the command name is too long */
            if (strlen(command_name) > strlen(keyword_table[i].name)) {
                printf("ERROR- Command name '%s' is too long\n", command_name);
                return UNDEFINED_KEYWORD;
            }

            /* If the command name is found, return the command key */
            return keyword_table[i].key;
        }
    }

    /* If the command name is not found, return UNDEFINED */
    return UNDEFINED_KEYWORD;
}


int get_command_opcode(keyword *keyword_table, int key) {
    int val;

    switch (command_number_by_key(keyword_table, key)) {
    case MOV:
        val = MOV_OPCODE;
        break;
    case CMP:
        val = CMP_OPCODE;
        break;
    case ADD:
        val = ADD_OPCODE;
        break;
    case SUB:
        val = SUB_OPCODE;
        break;
    case LEA:
        val = LEA_OPCODE;
        break;
    case CLR:
        val = CLR_OPCODE;
        break;
    case NOT:
        val = NOT_OPCODE;
        break;
    case INC:
        val = INC_OPCODE;
        break;
    case DEC:
        val = DEC_OPCODE;
        break;
    case JMP:
        val = JMP_OPCODE;
        break;
    case BNE:
        val = BNE_OPCODE;
        break;
    case RED:
        val = RED_OPCODE;
        break;
    case PRN:
        val = PRN_OPCODE;
        break;
    case JSR:
        val = JSR_OPCODE;
        break;
    case RTS:
        val = RTS_OPCODE;
        break;
    case STOP:
        val = STOP_OPCODE;
        break;
    case DATA:
    case STRING:
        val = UNKNOWN_NUMBER_OF_ARGUMENTS;
        break;
    default:
        val = NO_NEED_TO_DECODE;
        break;
    }

    return val;
}

int get_command_argument_count(int command_name) {
    switch (command_name) {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        return 3;
        break;

    case CLR:
    case NOT:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case RED:
    case PRN:
    case JSR:
        return 2;
        break;

    case RTS:
    case STOP:
        return 1;
        break;

    case DATA:
    case STRING:
    case ENTRY:
    case EXTERN:
        return UNKNOWN_NUMBER_OF_ARGUMENTS;
        break;

    default:
        return UNDEFINED;
        break;
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

register_name get_register_number(char *register_as_string) {
    size_t i;
    size_t length = strlen(register_as_string);
    int reg = UNDEFINED_REGISTER;

    /* Find the 'r' character in the string */
    for (i = 0; i < length && register_as_string[i] != 'r' && register_as_string[i] != '\0'; i++);

    /* If the 'r' character is not found, return an error */
    if (register_as_string[i] == '\0') return UNDEFINED_REGISTER;

    /* Extract the register number from the string */
    register_as_string += i + 1;
    reg = atoi(register_as_string);

    /* Check if the register number is valid */
    if (reg < R0 || reg > R7) return UNDEFINED_REGISTER;

    return reg;
}


validation_state label_name_is_valid(label_table *_label_table, char *_buffer, keyword *keywords_table, macro_table *_macro_table, status *entry_or_ext) {
    int i;
    macro *result_macro = NULL;
    keyword *result_keyword = NULL;
    char *label_name = _buffer;

    /* Check if the label name is NULL or too long */
    if (_buffer == NULL) {
        printf("Error- Checking a NULL label name.\n");
        return invalid;
    }

    if (strlen(label_name) > MAX_LABEL_LENGTH) {
        printf("ERROR- Label name %s is too long. Max label length is %d.\n", label_name, MAX_LABEL_LENGTH);
        return invalid;
    }



    /*Check if the label name contains only alphabetical characters and digits */
    for (i = 0; label_name[i] != '\0'; i++) {
        if (!(isalpha(label_name[i]) || isdigit(label_name[i]))) {
            printf("ERROR- Label name %s contains invalid characters.\n", label_name);
            return invalid;
        }
    }

    /* Check if the label name is a keyword */
    result_keyword = get_keyword_by_name(keywords_table, label_name);
    if (result_keyword != NULL) {
        printf("ERROR- Label name %s cannot be a keyword.\n", label_name);
        return invalid;
    }

    /* Check if the label name is a macro name */
    result_macro = get_macro(_macro_table, label_name);
    if (result_macro != NULL) {
        printf("ERROR- Label name '%s' cannot be the same as the '%s' macro name.\n", label_name, result_macro->name);
        return invalid;
    }

    /* Check if the label name is already in the label table */
    if (get_label(_label_table, label_name) != NULL && (*entry_or_ext) == NEITHER_EXTERN_NOR_ENTRY) {
        printf("ERROR- Label name '%s' is already in the label table.\n", label_name);
        return invalid;
    }

    return valid;
}

char *extract_label_name_from_instruction(char **_buffer, status *_entry_or_external) {
    char *label_name = NULL;
    char *copy_of_label_name = NULL;
    char *instruction_copy = NULL;
    char *instruction_copy_ptr = NULL;
    char *instruction_copy_ptr2 = NULL;
    bool contains_entry = false;
    bool contains_extern = false;
    int label_name_len = 0;
    *_entry_or_external = NEITHER_EXTERN_NOR_ENTRY;
    size_t chars_copied = 0;

    /*Check if the line contains the .entry or .extern directive */
    if (strstr(*(_buffer), ".entry") != NULL) {
        contains_entry = true;
        *_entry_or_external = CONTAINS_ENTRY;
    }
    if (strstr(*(_buffer), ".extern") != NULL) {
        contains_extern = true;
        *_entry_or_external = CONTAINS_EXTERN;
    }

    if (contains_entry && contains_extern) {
        printf("ERROR- Line cannot contain both '.entry' and '.extern' directives.\n");
        *_entry_or_external = NEITHER_EXTERN_NOR_ENTRY;
        return NULL;
    }


    /* If the instruction contains the '.entry' or '.extern' directive, it does not have the ':' character */
   /* Simply allocate a new name buffer, copy the label name to it and return it */
    if (contains_entry || contains_extern) {
        label_name = skip_ent_or_ext(*_buffer);
        label_name = trim_whitespace(label_name);
        instruction_copy_ptr = label_name;
        instruction_copy_ptr2 = label_name;

        while (instruction_copy_ptr2 && *(instruction_copy_ptr2) != '\0') {
            if (!isspace(*instruction_copy_ptr2))
                instruction_copy_ptr2++;
        }
        label_name_len = instruction_copy_ptr2 - instruction_copy_ptr + 1;
        copy_of_label_name = (char *)calloc(label_name_len, sizeof(char));

        if (copy_of_label_name == NULL) {
            free(*_buffer);
            (*_buffer) = NULL;
            label_name = NULL;
            printf("ERROR- Failed to allocate memory for copy of label name.\n");
            return NULL;
        }
        chars_copied = 0;
        while (chars_copied < label_name_len && instruction_copy_ptr && instruction_copy_ptr <= instruction_copy_ptr2) {
            copy_of_label_name[chars_copied] = instruction_copy_ptr[chars_copied];
            chars_copied++;
        }

        return copy_of_label_name;
    }

    label_name = *(_buffer);
    instruction_copy = NULL;

    /* Allocate memory for a copy of the instruction */
    instruction_copy = (char *)calloc(strlen(*(_buffer)) * PADDING, sizeof(char));
    if (instruction_copy == NULL) {
        free(*_buffer);
        *(_buffer) = NULL;
        free(copy_of_label_name);
        printf("ERROR- Failed to allocate memory for instruction copy.\n");
        return NULL;
    }
    trim_whitespace(label_name);

    /* Copy the instruction to a new buffer */
    strcpy(instruction_copy, label_name);

    /* Find the ':' character in the instruction */
    instruction_copy_ptr = instruction_copy;
    instruction_copy_ptr2 = instruction_copy;

    while (*(instruction_copy_ptr2) != '\0') {

        /* If the ':' character is found, return the string up to the ':' character */
        if (*(instruction_copy_ptr2) == ':') {
            /* Null terminate the label name */
            *(instruction_copy_ptr2) = '\0';
            label_name_len = strlen(instruction_copy_ptr) + 1;
            label_name = (char *)calloc(label_name_len, sizeof(char));
            if (label_name == NULL) {
                free(instruction_copy);
                free(*_buffer);
                (*_buffer) = NULL;
                label_name = NULL;
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


void print_label_table(label_table *_label_table) {
    int i;
    char entry[] = " .entry";
    char external[] = " .extern";
    char none[] = "No Directives";
    if (_label_table == NULL) {
        printf("ERROR- Trying to print a NULL label table.\n");
        return;
    }
    printf("\n##################################################\n");
    printf("Label table:\n");

    printf("\n-----------------------------------\n");

    for (i = 0; i < _label_table->size; i++) {
        printf("Name: %s \n", _label_table->labels[i]->name);
        printf("Key: %d \n", _label_table->labels[i]->key);
        if (_label_table->labels[i]->is_entry) {
            printf("Directives: %s \n", entry);
        }
        else if (_label_table->labels[i]->is_extern) {
            printf("Directives: %s \n", external);
        }
        else {
            printf("Directives: %s \n", none);
        }
        printf("Instruction line: %lu \n", _label_table->labels[i]->instruction_line);
        if (_label_table->labels[i]->address != 0) {
            printf("Address: %lu \n", _label_table->labels[i]->address);
        }
        printf("\n-----------------------------------\n");

    }
    printf("End of label table\n");
    printf("\n##################################################\n");
}


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


label *new_empty_label(label **new_label) {
    if (!(*new_label = (label *)malloc(sizeof(label)))) {
        return NULL;
    }

    /* Initialize the label fields */
    (*new_label)->key = 0;
    (*new_label)->instruction_line = 0;
    (*new_label)->address = 0;
    (*new_label)->size = 0;
    (*new_label)->is_entry = false;
    (*new_label)->is_extern = false;

    return *new_label;
}

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

void label_update_fields(label **new_label, char *label_name, int line_counter, status _entry_or_external, int address) {
    static int label_key = FIRST_KEY;
    /* Fill label name */
    strcpy((*new_label)->name, label_name);
    (*new_label)->key = label_key;
    label_key++;
    (*new_label)->instruction_line = line_counter;
    (*new_label)->address = address;
    (*new_label)->size = 0;

    if (_entry_or_external == CONTAINS_ENTRY) {
        (*new_label)->is_entry = true;
    }
    else if (_entry_or_external == CONTAINS_EXTERN) {
        (*new_label)->is_extern = true;
    }
    else {
        (*new_label)->is_entry = false;
        (*new_label)->is_extern = false;
    }

}

label *get_label(label_table *_label_table, char *label_name) {
    size_t i;

    for (i = 0; (i < _label_table->size) && (_label_table->labels[i] != NULL); i++) {
        if (!strcmp(_label_table->labels[i]->name, label_name)) {
            return _label_table->labels[i];
        }
    }
    return NULL;
}

label *get_label_by_key(label_table *_label_table, int key) {
    int i;

    if (_label_table == NULL) {
        printf("Trying to find a lable in a NULL label table.\n");
        return NULL;
    }

    for (i = 0;i < _label_table->size;i++) {
        if (_label_table->labels[i]->key == key) {
            return _label_table->labels[i];
        }
    }

}

void destroy_label(label **_label) {
    free(*_label);
    *_label = NULL;
}

void destroy_label_table(label_table **_label_table) {
    size_t i;
    if (_label_table == NULL || *(_label_table) == NULL) return;
    for (i = 0; i < (*_label_table)->size; i++) {
        destroy_label(&(*_label_table)->labels[i]);
    }
    free((*_label_table)->labels);
    free(*_label_table);
    *_label_table = NULL;
}

keyword *fill_keyword_table() {
    int i;
    char reg_name[3] = { 0 };
    keyword *keywords_table = NULL;

    /* Operation keywords */
    char *operation_keywords[] = {
       "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
    };
    int operation_keys[] = {
      MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP
    };
    int operation_lengths[] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4
    };

    /* Directive keywords */
    char *directive_keywords[] = {
        ".data", ".string", ".entry", ".extern"
    };
    int directive_keys[] = {
        DATA, STRING, ENTRY, EXTERN
    };
    int directive_lengths[] = {
        5, 7, 6, 7
    };

    /* Macro keywords */
    char *macro_keywords[] = {
        "macr", "endmacr"
    };
    int macro_keys[] = {
        MACR, ENDMACR
    };
    int macro_lengths[] = {
        4, 7
    };

    keywords_table = (keyword *)calloc(KEYWORD_TABLE_LENGTH, sizeof(keyword));
    if (keywords_table == NULL) {
        printf("Error while allocating memory for keyword table. Exiting...\n");
        return NULL;
    }

    /* Register keywords */
    for (i = 0; i < REGISTER_KEYWORDS; i++) {
        sprintf(reg_name, "r%d", i);
        strcpy(keywords_table[i].name, reg_name);
        keywords_table[i].key = R0 + i;
        keywords_table[i].length = 2;
    }


    /* Fill the keywords table */
    for (i = 0; i < OPERATION_KEYWORDS; i++) {
        strcpy(keywords_table[i + REGISTER_KEYWORDS].name, operation_keywords[i]);
        keywords_table[i + REGISTER_KEYWORDS].key = operation_keys[i];
        keywords_table[i + REGISTER_KEYWORDS].length = operation_lengths[i];
    }

    for (i = 0; i < MACRO_KEYWORDS; i++) {
        strcpy(keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS].name, macro_keywords[i]);
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS].key = macro_keys[i];
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS].length = macro_lengths[i];
    }

    for (i = 0; i < DIRECTIVE_KEYWORDS; i++) {
        strcpy(keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS + MACRO_KEYWORDS].name, directive_keywords[i]);
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS + MACRO_KEYWORDS].key = directive_keys[i];
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS + MACRO_KEYWORDS].length = directive_lengths[i];
    }

    return keywords_table;
}

label_table *fill_label_table(char *am_filename, macro_table *m_table, keyword *keywords_table) {
    char *instruction_buffer = NULL; /* Buffer to hold each line */
    char *label_name = NULL;
    int lines_in_file = 0;
    label_table *_label_table = NULL;
    FILE *am_file = NULL;
    label *new_label = NULL;
    validation_state _validation = invalid;
    status entry_or_external = NEITHER_EXTERN_NOR_ENTRY;


    instruction_buffer = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
    if (instruction_buffer == NULL) {
        printf("ERROR- Failed to allocate memory for instruction buffer\n");
        return NULL;
    }

    initialize_char_array(instruction_buffer);

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

    while (fgets(instruction_buffer, MAX_LINE_LENGTH, am_file)) { /* Read every line from the .am file */
        label_name = NULL;
        entry_or_external = NEITHER_EXTERN_NOR_ENTRY;

        /* Skip empty lines */
        if (is_empty_line(instruction_buffer)) {
            continue;
        }
        /* Skip comment lines */
        if (instruction_buffer[0] == ';') {
            continue;
        }

        lines_in_file++;
        instruction_buffer = trim_whitespace(instruction_buffer);

        /* Check if the line contains a label definition. if it does, save the label name */
        label_name = extract_label_name_from_instruction(&instruction_buffer, &entry_or_external);

        if (instruction_buffer == NULL) {
            printf("Cannot continue filling the label table. Exiting...\n");
            destroy_label_table(&_label_table);
            destroy_keyword_table(&keywords_table);
            free(instruction_buffer);
            fclose(am_file);
        }

        /* If the line does not contain a label definition, continue to the next line */
        if (label_name == NULL) {
            continue;
        }


        _validation = label_name_is_valid(_label_table, label_name, keywords_table, m_table, &entry_or_external);

        if (_validation != valid) {
            printf("ERROR- Invalid label name '%s' in file '%s' on line #%d\n", label_name, am_filename, lines_in_file + 1);
            printf("Cannot continue filling the label table. Exiting...\n");
            destroy_label_table(&_label_table);
            destroy_keyword_table(&keywords_table);
            free(instruction_buffer);
            fclose(am_file);
            return NULL;
        }

        /* If the line contains a label definition, check if the label name is valid */
        new_label = new_empty_label(&new_label);

        if (new_label == NULL) {
            printf("ERROR- Failed to allocate memory for new label.\nFile name: '%s'.\n Exiting...\n", am_filename);
            destroy_label_table(&_label_table);
            destroy_keyword_table(&keywords_table);
            macro_table_destructor(&m_table);
            free(instruction_buffer);
            fclose(am_file);
            return NULL;
        }

        /* Fill the label data and keep the address as UNDEFINED until the 2nd pass */
        label_update_fields(&new_label, label_name, lines_in_file, entry_or_external, 0);
        _label_table = insert_label(_label_table, new_label);

        if (_label_table == NULL) {
            printf("ERROR- Failed to insert label '%s' into label table.\nFile name: '%s'.\n Exiting...\n", label_name, am_filename);
            destroy_label_table(&_label_table);
            destroy_keyword_table(&keywords_table);
            macro_table_destructor(&m_table);
            free(instruction_buffer);
            fclose(am_file);
            return NULL;
        }

    }

    free(instruction_buffer);
    fclose(am_file);
    return _label_table;
}

addressing_method get_addressing_method(char *sub_inst, label_table *_label_table) {
    int i;

    /* case 0 */
    if (sub_inst[0] == '#') {
        if (sub_inst[1] == '\0') {
            return UNDEFINED;
        }
        if (sub_inst[1] != '-' && sub_inst[1] != '+' && !isdigit(sub_inst[1])) {
            return UNDEFINED;
        }
        if (sub_inst[1] == '-' || sub_inst[1] == '+') {
            if (!isdigit(sub_inst[2])) {
                return UNDEFINED;
            }
        }
        for (i = 2; i < (int)strlen(sub_inst); i++) {
            if (!isdigit(sub_inst[i])) {
                return UNDEFINED;
            }
        }
        return IMMEDIATE;
    }

    /* case 1 */
    for (i = 0; i < _label_table->size; i++) {
        if (!strcmp(_label_table->labels[i]->name, sub_inst)) {
            return DIRECT;
        }
    }

    /* case 2 */
    if (sub_inst[0] == '*') {
        if (sub_inst[1] == '\0' || sub_inst[1] != 'r') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[2] == '\0' || sub_inst[2] < '0' || sub_inst[2] > '7') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[3] != '\0') {
            return UNDEFINED_METHOD;
        }
        return INDIRECT_REGISTER;
    }

    /* case 3 */

    if (sub_inst[0] == 'r') {
        if (sub_inst[1] == '\0' || sub_inst[1] < '0' || sub_inst[1] > '7') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[2] != '\0') {
            return UNDEFINED_METHOD;
        }
        return DIRECT_REGISTER;
    }

    /* else */
    return UNDEFINED_METHOD;
}

void destroy_keyword_table(keyword **_keyword_table) {
    int i;
    if (_keyword_table == NULL || (*_keyword_table) == NULL) return;

    free(*_keyword_table);
    (*_keyword_table) = NULL;
}

