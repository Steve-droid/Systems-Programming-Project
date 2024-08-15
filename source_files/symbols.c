#include "symbols.h"
#define PADDING 2
#define INITIAL_CAPACITY 15
#define UNSET -1

/**
 *@brief Creates a table of keywords and their corresponding keys
 *
 * @return keyword* A table of keywords and their corresponding keys
 */
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
        print_system_error(NULL, NULL, m11_create_keyword_table);
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



label_table *fill_label_table(char *am_filename, char *as_filename, macro_table *m_table, keyword *keywords_table, int *syntax_errors) {
    FILE *am_file_ptr = NULL;
    label_table *_label_table = NULL;
    syntax_state *state = NULL;
    int next_key = 0;

    state = create_syntax_state();
    if (state == NULL) {
        print_system_error(NULL, NULL, m2_syntax_state);
        return NULL;
    }

    state->as_filename = as_filename;

    /* Open the .am file for reading lines */
    am_file_ptr = my_fopen(am_filename, "r");
    if (am_file_ptr == NULL) {
        quit_label_parsing(NULL, &state, NULL);
        return NULL;
    }

    /* Initialize the label table */
    _label_table = create_label_table(&_label_table);
    if (_label_table == NULL) {
        quit_label_parsing(NULL, &state, am_file_ptr);
        return NULL;
    }

    state->line_number = -1; /* lines start from 0*/
    state->k_table = keywords_table;
    state->m_table = m_table;
    state->l_table = _label_table;

    while (fgets(state->buffer, MAX_LINE_LENGTH, am_file_ptr)) { /* Read every line from the .am file */
        /* Create a new label object */
        state->_label = create_label();
        if (state->_label == NULL) {
            quit_label_parsing(&_label_table, &state, am_file_ptr);
            return NULL;
        }

        state->_label->key = next_key++;

        /* Save the buffer without the offset, so we don't lose the original buffer */
        state->buffer_without_offset = state->buffer;

        state->buffer = trim_whitespace(state->buffer);

        /* Skip empty/comment lines */
        if (is_empty_line(state->buffer) || state->buffer[0] == ';') {
            destroy_label(&state->_label);
            state->buffer = state->buffer_without_offset;
            reset_syntax_state(state);
            continue;
        }

        state->line_number++;

        /* Check if the line contains a label definition. If so, update the label name in the label object */
        extract_label_name_from_instruction(state);

        if (state->extern_or_entry == ERR || state->_label->ignore || state->label_name_detected == false) {
            destroy_label(&state->_label);
            state->buffer = state->buffer_without_offset;
            reset_syntax_state(state);
            continue;
        }


        state->tmp_arg = state->_label->name;
        if (validate_label_name(state) != valid) {
            (*syntax_errors)++;
            destroy_label(&state->_label);
            state->buffer = state->buffer_without_offset;
            reset_syntax_state(state);
            continue;
        }


        /* Fill the label data and keep the address as UNDEFINED until the 2nd pass */
        state->_label->instruction_line = state->line_number;



        insert_label(_label_table, &state->_label);

        if (_label_table == NULL) {
            destroy_label(&state->_label);
            state->buffer = state->buffer_without_offset;
            quit_label_parsing(&_label_table, &state, am_file_ptr);
            return NULL;
        }


        state->buffer = state->buffer_without_offset;
        reset_syntax_state(state);

    }

    reset_syntax_state(state);

    quit_label_parsing(NULL, &state, am_file_ptr);
    return _label_table;
}

keyword *get_keyword_by_name(keyword *keyword_table, char *name) {
    int i;
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (strcmp(keyword_table[i].name, name) == 0) {
            return &keyword_table[i];
        }
    }
    return NULL;
}

label *get_label_by_name(label_table *_label_table, char *label_name) {
    size_t i;

    if (_label_table == NULL || _label_table->labels == NULL) {
        return NULL;
    }

    for (i = 0; (i < _label_table->size) && (_label_table->labels[i] != NULL); i++) {
        if (!strcmp(_label_table->labels[i]->name, label_name)) {
            return _label_table->labels[i];
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

    return NULL;

}

int get_command_argument_count(int command_key) {
    switch (command_key) {
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

addressing_method get_addressing_method(syntax_state *state, char *sub_inst, label_table *_label_table) {
    int i;

    /* case 0 */
    if (sub_inst[0] == '#') {
        if (sub_inst[1] == '\0') {
            return UNDEFINED_METHOD;
        }
        if (sub_inst[1] != '-' && sub_inst[1] != '+' && !isdigit(sub_inst[1])) {
            print_syntax_error(state, e42_imm_val_not_digit);
            return UNDEFINED_METHOD;
        }
        if (sub_inst[1] == '-' || sub_inst[1] == '+') {
            if (!isdigit(sub_inst[2])) {
                return UNDEFINED;
            }
        }
        for (i = 2; i < (int)strlen(sub_inst); i++) {
            if (!isdigit(sub_inst[i])) {
                print_syntax_error(state, e46_imm_inv_after_pm);
                return UNDEFINED_METHOD;
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
            print_syntax_error(state, e43_inval_indirect_reg);
            return UNDEFINED_METHOD;
        }
        if (sub_inst[2] == '\0' || sub_inst[2] < '0' || sub_inst[2] > '7') {
            print_syntax_error(state, e44_indirect_reg_number_not_in_range);
            return UNDEFINED_METHOD;
        }
        if (sub_inst[3] != '\0') {
            print_syntax_error(state, e47_ext_chars_after_indirect_reg);
            return UNDEFINED_METHOD;
        }
        return INDIRECT_REGISTER;
    }

    /* case 3 */

    if (sub_inst[0] == 'r') {
        if (sub_inst[1] == '\0' || sub_inst[1] < '0' || sub_inst[1] > '7') {
            print_syntax_error(state, e50_direct_reg_num_not_in_range);
            return UNDEFINED_METHOD;
        }
        if (sub_inst[2] != '\0') {
            print_syntax_error(state, e49_ext_chars_after_direct_reg);
            return UNDEFINED_METHOD;
        }
        return DIRECT_REGISTER;
    }


    /* else - not direct */

    print_syntax_error(state, e51_unknown_label);
    return UNDEFINED_METHOD;
}

keyword_name identify_command(syntax_state *state, label_table *_label_table, keyword *keyword_table) {
    int i;
    char *_instruction = NULL;
    char command_name[MAX_LINE_LENGTH] = { 0 };
    size_t buffer_length = strlen(state->buffer);
    int command_found = false;


    if (state == NULL || _label_table == NULL || keyword_table == NULL) {
        return UNDEFINED;
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
                state->k_table = keyword_table;
                state->index = i;
                print_syntax_error(state, n1_prefix_op);
                return UNDEFINED;
            }

            /* If the command name is found, return the command key */
            return keyword_table[i].key;
        }
    }

    /* If the command name is not found, return UNDEFINED */
    return UNDEFINED;
}

opcode get_command_opcode(keyword *keyword_table, int key) {
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
        val = NONE;
        break;
    }

    return val;
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


int compare_labels(const void *a, const void *b) {
    label *label_a = *(label **)a;
    label *label_b = *(label **)b;

    if (label_a->key < label_b->key) {
        return -1;
    }
    if (label_a->key > label_b->key) {
        return 1;
    }
    return 0;
}

void sort_label_table(label_table *_label_table, label *_label) {
    size_t i;


    if (_label_table == NULL || _label == NULL) {
        return;
    }

    for (i = 0; i < _label_table->size; i++) {
        if (_label_table->labels[i] == _label) {
            break;
        }
    }

    if (_label_table->size == 0) {
        return;
    }

    qsort(_label_table->labels, _label_table->size, sizeof(label *), compare_labels);

}


void extract_label_name_from_instruction(syntax_state *state) {
    size_t chars_copied = 0;
    char *ptr_trailing = NULL;
    char *ptr_leading = NULL;
    int entry_decleration = false;
    int extern_decleration = false;
    int label_name_len = 0;
    label *tmp_label = NULL;


    /*Check if the line contains the .entry or .extern directive */
    if (strstr(state->buffer, ".entry") != NULL) {
        entry_decleration = true;
    }
    if (strstr(state->buffer, ".extern") != NULL) {
        extern_decleration = true;
    }

    if (entry_decleration && extern_decleration) {
        print_syntax_error(state, e66_redef_directive);
        state->extern_or_entry = ERR;
        return;
    }



    /* If the instruction contains the '.entry' or '.extern' directive, it does not have the ':' character */
    /* Simply allocate a new name buffer, copy the label name to it and return it */
    if (entry_decleration || extern_decleration) {
        state->buffer = skip_ent_or_ext(state->buffer);
        state->buffer = trim_whitespace(state->buffer);
        ptr_leading = state->buffer;
        ptr_trailing = ptr_leading;

        /*
        *Check if the declaration of the label precedes the directive
        * If so, we simply update the label in the label table and return
        */
        tmp_label = get_label_by_name(state->l_table, state->buffer);
        if (tmp_label != NULL) {
            if (entry_decleration) {
                tmp_label->declared_as_entry = true;

                /* We don't need to create a new label for this entry or extern directive if the label already exists */
                state->_label->ignore = true;
                return;
            }
        }

        /*
        * If the directive precedes the label definition, we need to create a new label with empty fields and update the label name
        * When we reach the label definition, we will update the label fields
        */

        /* Find the end of the label name by searching for the first whitespace character */
        while (ptr_leading != NULL && *(ptr_leading) != '\0') {
            if (!isspace(*ptr_leading)) {
                ptr_leading++;
            }
        }

        /* Calculate the length of the label name */
        label_name_len = ptr_leading - ptr_trailing + 1;

        /* Copy the label name to the new label */
        chars_copied = 0;
        while (chars_copied < label_name_len && ptr_trailing && ptr_trailing <= ptr_leading) {
            state->_label->name[chars_copied] = ptr_trailing[chars_copied];
            chars_copied++;
        }

        /* Turn on the flag that indicates that the label needs to be updated */
        state->_label->missing_definition = true;
        state->_label->declared_as_entry = entry_decleration;
        state->_label->declared_as_extern = extern_decleration;
        state->label_name_detected = true;

        return;
    }




    /*Check if the instruction contains a label definition */
    ptr_trailing = state->buffer;
    ptr_leading = strchr(state->buffer, ':');

    if (ptr_leading == NULL) {
        return;
    }

    state->label_name_detected = true;

    /* Calculate the length of the label name */
    label_name_len = ptr_leading - ptr_trailing;

    /* Copy the label name to the new label */
    chars_copied = 0;
    while (chars_copied < label_name_len && ptr_trailing && ptr_trailing < ptr_leading) {
        state->_label->name[chars_copied] = ptr_trailing[chars_copied];
        chars_copied++;
    }

    /* Null-terminate the label name */
    state->_label->name[chars_copied] = '\0';

    /* Check if a label with the same name already exists in the label table */
    if (state->_label != NULL && !entry_decleration && !extern_decleration) {
        tmp_label = get_label_by_name(state->l_table, state->_label->name);

        /* If the label name is found as a directive, update the label in the label table and don't create a new label */
        if (tmp_label != NULL && tmp_label->missing_definition) {
            if (tmp_label->declared_as_entry) {
                tmp_label->instruction_line = state->line_number;
                tmp_label->missing_definition = false;
                tmp_label->key = state->_label->key;
                state->_label->ignore = true;

                /* Bubble the label to the top of the label table */
                sort_label_table(state->l_table, tmp_label);


                return;

            }
        }
    }


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

validation_state validate_label_name(syntax_state *state) {
    int i;
    macro *result_macro = NULL;
    keyword *result_keyword = NULL;
    char *label_name = NULL;
    label *result_label = NULL;

    /* Check if the label name is NULL or too long */
    if (state == NULL) {
        return invalid;
    }

    label_name = state->tmp_arg;

    if (strlen(label_name) > MAX_LABEL_LENGTH) {
        print_syntax_error(state, e60_label_name_is_too_long);
        return invalid;
    }



    /*Check if the label name contains only alphabetical characters and digits */
    for (i = 0; label_name[i] != '\0' && label_name[i] != ':'; i++) {
        if (!(isalpha(label_name[i]) || isdigit(label_name[i]))) {
            if (isspace(label_name[i])) {
                print_syntax_error(state, e64_whitespace_between_label_and_colon);
                return invalid;
            }
            print_syntax_error(state, e63_label_name_not_alphanumeric);
            return invalid;
        }
    }

    /* Check if the label name is a keyword */
    result_keyword = get_keyword_by_name(state->k_table, label_name);
    if (result_keyword != NULL) {
        print_syntax_error(state, e61_label_name_is_keyword);
        return invalid;
    }

    /* Check if the label name is a macro name */
    result_macro = get_macro(state->m_table, label_name);
    if (result_macro != NULL) {
        print_syntax_error(state, e62_label_name_is_macro);
        return invalid;
    }

    /* Check if the label name is already in the label table */
    result_label = get_label_by_name(state->l_table, label_name);
    if (result_label != NULL) {

        print_syntax_error(state, e59_label_redef);
        return invalid;
    }

    return valid;
}

label *create_label() {
    label *new_label = NULL;

    new_label = (label *)calloc(1, sizeof(label));
    if (new_label == NULL) {
        print_system_error(NULL, NULL, m13_create_label);
        return NULL;
    }

    new_label->address = 0;
    new_label->key = UNSET;
    new_label->instruction_line = UNSET;
    new_label->declared_as_entry = false;
    new_label->declared_as_extern = false;
    new_label->ignore = false;
    new_label->missing_definition = false;


    return new_label;


}

label_table *create_label_table(label_table **new_label_table) {



    if (!(*new_label_table = (label_table *)malloc(sizeof(label_table)))) {
        return NULL;
    }
    (*new_label_table)->size = 0;
    (*new_label_table)->capacity = 1;
    (*new_label_table)->labels = (label **)calloc(1, sizeof(label *));
    if ((*new_label_table)->labels == NULL) {
        free(*new_label_table);
        return NULL;
    }
    return *new_label_table;
}

status insert_label(label_table *_label_table, label **_label) {
    label *tmp_label = NULL;



    tmp_label = get_label_by_name(_label_table, (*_label)->name);
    if (tmp_label != NULL) {
        return DUPLICATED;
    }

    if (_label_table->size == _label_table->capacity) {
        _label_table->capacity += 1;
        _label_table->labels = (label **)realloc(_label_table->labels, _label_table->capacity * sizeof(label *));
        if (_label_table->labels == NULL) {
            free(_label_table);
            return STATUS_ERROR;
        }
    }

    _label_table->labels[_label_table->size] = (*_label);
    _label_table->size++;
    return STATUS_OK;
}

void destroy_keyword_table(keyword **_keyword_table) {
    if (_keyword_table == NULL || (*_keyword_table) == NULL) return;

    free(*_keyword_table);
    (*_keyword_table) = NULL;
}

void destroy_label(label **_label) {
    if (_label == NULL) return;

    free(*_label);
    *_label = NULL;
}

void destroy_label_table(label_table **_label_table) {
    size_t i;
    if (_label_table == NULL || *(_label_table) == NULL) return;
    for (i = 0; i < (*_label_table)->capacity; i++) {
        destroy_label(&(*_label_table)->labels[i]);
    }
    free((*_label_table)->labels);
    free(*_label_table);
    *_label_table = NULL;
}

void print_label_table(label_table *_label_table) {
    int i;
    char entry[] = " .entry";
    char external[] = " .extern";
    char none[] = "None";
    if (_label_table == NULL) {
        printf("\nTrying to print a NULL label table.\n");
        return;
    }
    printf("\n######################################################\n");
    printf("Printing Label Table:");
    printf("\n######################################################\n\n");

    for (i = 0; i < _label_table->size; i++) {
        printf("Name: %s \n", _label_table->labels[i]->name);
        if (_label_table->labels[i]->key != 0 && _label_table->labels[i]->key != -1)
            printf("Key: %d \n", _label_table->labels[i]->key);

        if (_label_table->labels[i]->declared_as_entry) {
            printf("Directives: %s \n", entry);
        }
        else if (_label_table->labels[i]->declared_as_extern) {
            printf("Directives: %s \n", external);
        }
        else {
            printf("Directives: %s \n", none);
        }
        if (_label_table->labels[i]->instruction_line != 0)
            printf("Instruction line: %lu \n", _label_table->labels[i]->instruction_line);
        if (_label_table->labels[i]->address != 0 && _label_table->labels[i]->address != -1) {
            printf("Address: %lu \n", _label_table->labels[i]->address);
        }
        printf("\n-----------------------------------\n");

    }
    printf("\n######################################################\n");
    printf("End Of Label Table");
    printf("\n######################################################\n\n\n");


}
