#include "keyword.h"

/**
 *@brief This function is used to fill the keywords table with the operation, directive, and register keywords.
 * Here are the keywords that are filled in the table:
 * 1. Registers: r0, r1, r2, r3, r4, r5, r6, r7.
 * 2. Operation keywords: mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop.
 * 3. Directive keywords: .data, .string, .entry, .extern.
 * 4. Macro keywords: macr, endmacr.
 * Each keyword has a key that represents its type, and a length that represents the length of the keyword.
 * @return keyword* A pointer to the filled keywords table.
 */
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

/**
 *@brief This function is used to get a keyword from the keywords table.
 * The function searches for a keyword in the keywords table by its name.
 * @param keywords_table A pointer to the keywords table.
 * @param name The name of the keyword to search for.
 * @return keyword* A pointer to the keyword if found, NULL otherwise.
 */
keyword *get_keyword_by_name(keyword *keyword_table, char *name) {
    int i;
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (strcmp(keyword_table[i].name, name) == 0) {
            return &keyword_table[i];
        }
    }
    return NULL;
}

/**
 *@brief This function is used to get a keyword from the keywords table by its key.
 * The function searches for a keyword in the keywords table by its key.
 * @param keywords_table A pointer to the keywords table.
 * @param key The key of the keyword to search for.
 * @return keyword* A pointer to the keyword if found, NULL otherwise.
 */
keyword *get_keyword_by_key(keyword *keyword_table, int key) {
    int i;
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (keyword_table[i].key == key) {
            return &keyword_table[i];
        }
    }
    return NULL;
}

/**
 * @brief Identifies and returns the keyword key for a command in the given line of assembly code.
 *
 * This function processes a line of assembly code to identify the command present in the line.
 * It performs the following steps:
 * - Skips any labels and leading spaces in the line.
 * - Extracts the command name from the line.
 * - Compares the extracted command name against the keywords in the keyword table.
 * - Returns the keyword key if a match is found, otherwise returns UNDEFINED.
 *
 * @param line The line of assembly code to process.
 * @param _label_table The table containing label information.
 * @param keyword_table The table containing keyword information.
 * @param current_line The current line number being processed.
 * @return The keyword key if the command is found, otherwise UNDEFINED.
 */
int identify_command(char *line, label_table *_label_table, keyword *keyword_table, int current_line) {
    int i;
    char *line_after_label = NULL;
    char command_name[MAX_LINE_LENGTH];

    /* Get the portion of the line after any labels */
    line_after_label = pointer_after_label(line, _label_table, current_line);
    remove_prefix_spaces(line_after_label); /* Skip unnecessary spaces */

    /* Extract the command name from the line */
    for (i = 0; line_after_label[i] != '\n' && !isspace(line_after_label[i]); i++) {
        command_name[i] = line_after_label[i];
    }
    command_name[i] = '\0';

    /* Compare the extracted command name against the keyword table */
    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (!strncmp(command_name, keyword_table[i].name, strlen(keyword_table[i].name))) {
            if (strlen(command_name) > strlen(keyword_table[i].name)) {
                return UNDEFINED;
            }
            return keyword_table[i].key;
        }
    }
    return UNDEFINED;
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


/**
 * @brief Determines the number of arguments required for a given command.
 *
 * This function takes a command name as input and returns the number of arguments that
 * the command requires. The number of arguments is based on predefined command names.
 *
 * @param command_name The name of the command to check.
 * @return The number of arguments required for the command, or special constants like UNKNOWN_NUMBER_OF_ARGUMENTS or UNDEFINED.
 */

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