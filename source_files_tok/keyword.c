#include "keyword.h"

keyword *fill_keyword_table() {
    int i;
    char reg_name[3] = { 0 };
    static keyword *keywords_table = NULL;

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

    keywords_table = (keyword *)malloc(KEYWORD_TABLE_LENGTH * sizeof(keyword));
    if (keywords_table == NULL) {
        printf("ERROR-ALLOCATION FAILED\n");
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


keyword_name identify_command(buffer_data *_buffer_data, label_table *_label_table, keyword *keyword_table) {
    int i;
    char *_instruction = NULL;
    char command_name[MAX_LINE_LENGTH];
    size_t command_length = strlen(_buffer_data->buffer);
    bool command_found = false;


    if (_buffer_data == NULL || _label_table == NULL || keyword_table == NULL) {
        printf("Tried to identify command with NULL arguments\n");
        return UNDEFINED_KEYWORD;
    }

    _instruction = _buffer_data->buffer;

    /* Extract the command name from the line */
    for (i = 0; i < command_length && _instruction[i] != '\n' && !isspace(_instruction[i]); i++) {
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