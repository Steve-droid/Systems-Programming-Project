#include "parser.h"

/*decode*/
int *proccess_assembly_code(char *am_filename, label_table *_label_table, keyword *keyword_table) {

    int *decoded = NULL;
    int **decoded_table = NULL;

    decoded_table = lex(am_filename, _label_table, keyword_table);
    decoded = convert_to_1D(decoded_table);
    printf("\nPOST-DECODED (LABEL PRESENTED AS ITS KEY):\n");
    print_array_in_binary(decoded);

    assign_label_addresses(decoded_table, _label_table);
    decoded = convert_to_1D(decoded_table);
    replace_label_keys_with_addresses(decoded, _label_table);

    free(decoded_table);
    return decoded;
}


int fill_pre_decoded_array(char *line, string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int command_name_key, label_table *_label_table, int current_line) {
    int command_number;
    char *line_from_first_letter = NULL;

    /*adding the first argument- the command (in array[0])*/
    command_number = command_number_by_key(keyword_table, command_name_key);
    strcpy(pre_decoded[0].data, keyword_table[command_number].name);

    /*update the line pointer from the rellevant cell*/
    line_from_first_letter = pointer_after_label(line, _label_table, current_line); /*skip the label (if it there) and spaces*/
    line_from_first_letter = &line_from_first_letter[keyword_table[command_number].length]; /*skip the command*/
    remove_prefix_spaces(line_from_first_letter); /*skip unnecessary spaces*/

    if (line_from_first_letter[0] == ',') { /*unnecessary comma between command and arguments*/
        printf("ERROR- An unnecessary comma between command and arguments\n");
        return false;
    }

    if (pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS) { /* keep all the arguments in array[1] */
        return process_fields_for_unknown_amount(line_from_first_letter, command_number, keyword_table, pre_decoded, _label_table);
    }
    return process_fields_for_known_amount(line_from_first_letter, pre_decoded, pre_decoded_size, _label_table);
}

int process_fields_for_unknown_amount(char *line, int command_number, keyword *keyword_table, string *pre_decoded, label_table *_label_table) {
    int i, j, comma, end_of_argument_by_space, end_of_argument, end_of_string, last_quatiotion_mark, first_quatiotion_mark, flag;

    comma = end_of_argument_by_space = end_of_argument = end_of_string = first_quatiotion_mark = last_quatiotion_mark = flag = false;

    for (i = 0, j = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        /* in case of .data */
        if (!strcmp(keyword_table[command_number].name, ".data")) {
            /*We need the data look like: int comma int comma int comma etc*/
            if (isspace(line[i]) && comma == false) { /*we collected a full number that overed with space*/
                end_of_argument_by_space = true;
            }
            remove_prefix_spaces(&line[i]);
            /*check wether the first char is a comma or not*/
            if (line[0] == ',') {
                printf("ERROR- Additional unnecessary comma before '.data' data\n");
                return false;
            }
            /*in case of invalid symbol*/
            if (!(isdigit(line[i]) || line[i] == '-' || line[i] == '+' || line[i] == ',' || line[i] == '\0' || line[i] == '\n')) {
                printf("ERROR- Invalid symbol in '.data' data\n");
                return false;
            }
            /*in case of a new number without a comma between the current and the last one*/
            if (end_of_argument_by_space == true && line[i] != ',' && line[i] != '\0' && line[i] != '\n') { /*its a valid symbol*/
                printf("ERROR- NO COMMA BETWEEN INTEGERS\n");
                return false;
            }
            /*in case of a series of commas without a number between them*/
            if (comma == true && line[i] == ',') {
                printf("ERROR- A series of commas without a number between them\n");
                return false;
            }

            /*The '.data' data is valid*/
            pre_decoded[1].data[j++] = line[i];
            pre_decoded[1].data[j] = '\0';

            /* upadte data for next loop */
            if (line[i] == ',') {
                comma = true;
            }
            else {
                end_of_argument_by_space = comma = false;
            }

            /*in case of problems after data collected*/
            if (line[i] == '\n' || line[i + 1] == '\n' || line[i] == '\0' || line[i + 1] == '\0') {
                if (comma == true) {  /*in case of comma after the last number*/
                    printf("ERROR- An unnecessary comma in the end of '.data' data\n");
                    return false;
                }
                if (validate_data_numbers(pre_decoded[1].data)) {
                    printf("ERROR- not a vaild integer\n");
                    return false;
                }
            }
        }

        /* in case of ".string" */
        else if (!strcmp(keyword_table[command_number].name, ".string")) {
            if (i == 0) {
                if (line[0] == '\"') { /* The first data of ".string" is " */
                    first_quatiotion_mark = true;
                }
                continue;
            }
            if (line[i] == '\"') {
                last_quatiotion_mark = true;
            }
            else {
                last_quatiotion_mark = false;
            }
            pre_decoded[1].data[j++] = line[i];
            pre_decoded[1].data[j] = '\0';

            if (line[i + 1] == '\n' || line[i + 1] == '\0') {
                if (first_quatiotion_mark == false && last_quatiotion_mark == false) { /* there are no " " */
                    printf("ERROR- There are no quatiotion marks at the the data in .string command\n");
                    fflush(stdout);
                    return false;
                }
                else if (first_quatiotion_mark == false && last_quatiotion_mark == true) {
                    printf("ERROR- There is no \" at the beginning of the data in .string command\n");
                    fflush(stdout);
                    return false;
                }
                else if (first_quatiotion_mark == true && last_quatiotion_mark == false) {
                    printf("ERROR- There is no \" at the end of the data in .string command\n");
                    fflush(stdout);
                    return false;
                }
                else { /*terminator instead of quatiotion mark*/
                    while (j > 0 && pre_decoded[1].data[j] != '\"') {
                        j--;
                    }
                    pre_decoded[1].data[j] = '\0';
                }
            }
        }
        else { /* in case of ".entry" , ".extern" */
            if (!(isalpha(line[i]) || isdigit(line[i]))) {
                printf("ERROR- Invalid character for label, no label with this name");
                return false;
            }
            if (end_of_argument == true && (!isspace(line[i]))) {
                printf("ERROR- Too many arguments");
                return false;
            }
            if (isspace(line[i]) || line[i + 1] == '\0' || line[i + 1] == '\n') {
                end_of_argument = true;
            }
            if (!isspace(line[i])) {
                pre_decoded[1].data[j++] = line[i];
                pre_decoded[1].data[j] = '\0';
            }
        }
    }
    /*check the label type (entry or extern) in label table and fill it*/
    /* create a new function - fill_label_type */
    if (!strcmp(keyword_table[command_number].name, ".entry") || !strcmp(keyword_table[command_number].name, ".extern")) {
        for (i = 0; i < _label_table->size; i++) {
            if (!strcmp(pre_decoded[1].data, _label_table->labels[i]->name)) {
                if (!strcmp(keyword_table[command_number].name, ".entry")) {
                    if (_label_table->labels[i]->entry_or_extern == EXTERN) {
                        printf("ERROR - DEFINED A LABEL BY ENTRY AND EXTERN");
                        return false;
                    }
                    _label_table->labels[i]->entry_or_extern = ENTRY;
                }
                else {/*extern*/
                    if (_label_table->labels[i]->entry_or_extern == ENTRY) {
                        printf("ERROR - DEFINED A LABEL BY ENTRY AND EXTERN");
                        return false;
                    }
                    _label_table->labels[i]->entry_or_extern = EXTERN;
                }
                flag = true;
            }
        }
        if (flag == false) {
            printf("ERROR- No such label name");
            return false;
        }
    }
    return true;
}

int process_fields_for_known_amount(char *line, string *pre_decoded, int pre_decoded_size, label_table *_label_table) {
    int i, j, k, comma;

    comma = false;

    for (i = 1; i < pre_decoded_size; i++) { /*loop for every argument*/
        j = 0;
        /*reached the end of the line without adding enough arguments*/
        if ((line[j] == '\0' || line[j] == '\n')) {
            printf("ERROR- There are less arguments than expected for the specific command\n");
            return false;
        }

        /*fill argument in the right place*/
        for (k = 0; line[j] != '\0' && line[j] != '\n' && !isspace(line[j]) && line[j] != ','; j++) {
            pre_decoded[i].data[k++] = line[j];
            comma = false;
        }
        pre_decoded[i].data[k] = '\0'; /*null terminator*/

        /*update to check errors between arguments*/
        line = &line[j];
        remove_prefix_spaces(line);

        /*a series of commas*/
        if (line[0] == ',' && comma == true) {
            printf("ERROR- A series of commas without a number between them\n");
            return false;
        }

        /*check about the comma between arguments and update line for the next loop*/
        if (line[0] == ',') {
            comma = true;
            line = &line[1];
            remove_prefix_spaces(line);
        }
        else if (i != pre_decoded_size - 1) {
            printf("ERROR- There is no comma between arguments\n");
            return false;
        }
        if (!validate_known_argument_data(pre_decoded[i].data, _label_table)) {
            printf("ERROR- The argument data is not valid");
            return false;
        }
    }

    for (i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        if (!isspace(line[i])) {
            printf("ERROR- Too many arguments for the specific command\n");
            return false;
        }
    }

    if (comma == true) {
        printf("ERROR- There is a comma after the last argument\n");
        return false;
    }

    return true;
}

bool validate_known_argument_data(char *str, label_table *_label_table) {
    int tmp;

    tmp = get_addressing_method(str, _label_table);

    if (tmp == 0 || tmp == 1 || tmp == 2 || tmp == 3) {
        return true;
    }
    return false;
}

bool validate_addressing_methods(int *binary_command, keyword *keyword_table, int key) {
    bool decision_source, decision_destination;

    decision_source = validate_source_argument_addressing(binary_command, keyword_table, key);
    decision_destination = validate_destination_argument_addressing(binary_command, keyword_table, key);

    return decision_source && decision_destination;
}

bool validate_source_argument_addressing(int *binary_command, keyword *keyword_table, int key) {
    int decision, i;

    switch (command_number_by_key(keyword_table, key)) {
        /* case 0 , 1 , 2 , 3 */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
        decision = false;
        for (i = OPCODE_LEN; i < (OPCODE_LEN + ADDRESSING_METHOD_LEN); i++) {
            if (binary_command[i] == 1) {
                decision = true;
            }
        }
        break;
        /* case 1 */
    case LEA:
        if (binary_command[OPCODE_LEN + (3 - 1)] != 1) { /*addresing method 1*/
            return false;
        }
        decision = true;

        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if (i != OPCODE_LEN + (3 - 1) && binary_command[i] == 1) {
                decision = false;
            }
        }
        break;
        /* case - no source */
    default: /* clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop */
        decision = true;
        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = false;
            }
        }
        break;
    }
    return decision;
}

bool validate_destination_argument_addressing(int *binary_command, keyword *keyword_table, int key) {

    int decision, i;

    switch (command_number_by_key(keyword_table, key)) {
        /* case 0 , 1 , 2 , 3 */
    case CMP:
    case PRN:
        decision = false;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = true;
            }
        }
        break;
        /* case 1 , 2 , 3*/
    case MOV:
    case ADD:
    case SUB:
    case LEA:
    case CLR:
    case NOT:
    case INC:
    case DEC:
    case RED:
        if (binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0)] == 1) { /*in case of 0 is on*/
            return false;
        }
        decision = false;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (i != OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0) && binary_command[i] == 1) {
                decision = true;
            }
        }
        break;
        /* case 1 , 2 */
    case JMP:
    case BNE:
    case JSR:
        /*in case of 1 and 2 is off*/
        if (binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 1)] != 1 &&
            binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 2)] != 1) {
            return false;
        }
        decision = true;
        /*in case of 0 or 3 is on*/
        if (binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0)] == 1 ||
            binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 3)] == 1) {
            decision = false;
        }

        break;
        /* case no destination */
    default: /* rts , stop */
        decision = true;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = false;
            }
        }
        break;
    }
    return decision;
}

int get_register_number(char *reg) {
    int i;

    for (i = 0; reg[i] != 'r' && reg[i] != '\0'; i++);

    return atoi(&reg[i + 1]);
}

/**
 *@brief Get a field in an instruction and return the addressing method of the arguments in the fields
 *
 * @param str The ins
 * @param label_table
 * @return int
 */
 /**
  *@brief Get a field in an instruction and return the addressing method of the arguments in the fields
  *
  * @param str The ins
  * @param label_table
  * @return int
  */
int get_addressing_method(char *str, label_table *_label_table) {
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
    for (i = 0; i < _label_table->size; i++) {
        if (!strcmp(_label_table->labels[i]->name, str)) {
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





















