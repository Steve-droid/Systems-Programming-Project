#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utilities.h"
#include "decode_to_string_array.h"
#include "decoding.h"
#include "label_table.h"
#include "decode_to_int_array.h"


int **decode_without_label_addresses(char *am_filename, label *label_table, keyword *keyword_table) {
    char line[MAX_LINE_LENGTH]; /* Buffer to hold each line */
    string *pre_decoded; /*An array that contains the arguments in each cell*/
    int command_name, pre_decoded_size, line_counter, post_decoded_table_size;
    int *post_decoded = NULL;
    int **post_decoded_table = NULL;

    post_decoded_table_size = 0;
    post_decoded_table = (int **)malloc((post_decoded_table_size + 1) * sizeof(int)); /* +1 for NULL terminator*/
    if (post_decoded_table == NULL) {
        printf("ERROR-ALLOCATION");
        return NULL;
    }
    post_decoded_table[0] = NULL;

    initialize_char_array(line);

    FILE *file = fopen(am_filename, "r"); /* Open the file for reading*/
    if (file == NULL) { /*PROBLEM-FILE NOT EXIST*/
        printf("Error- file doesnt open\n");
        free(post_decoded_table);
        return NULL;
    }

    command_name = pre_decoded_size = 0;
    line_counter = -1; /*first line is 0*/

    printf("PRE-DECODED:\n");

    while (fgets(line, sizeof(line), file)) { /* Read every line */
        if (is_empty_line(line)) {
            continue;
        }

        remove_prefix_spaces(line);
        if (line[0] == ';') {
            continue;
        }

        line_counter++;
        command_name = deciphering_command(line, label_table, keyword_table, line_counter); /*decipher the command name*/
        pre_decoded_size = arguments_amount_for_command(command_name); /*Returns the amount of cells that needed for every argument*/

        if (command_name == UNDEFINED || pre_decoded_size == UNDEFINED) { /*undefined command name*/
            printf("ERROR- undefined command name");
            free(post_decoded_table);
            return NULL;
        }

        /*creates the right size that needed to save the data*/
        if (pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS) { /*the array that contains unknown arguments */
            pre_decoded = (string *)malloc(2 * sizeof(string)); /*array[0]: command name , array[1]: command data*/
            if (pre_decoded == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                free(post_decoded_table);
                return NULL;
            }
        }
        else {
            pre_decoded = (string *)malloc(pre_decoded_size * sizeof(string)); /* every cell contains a word*/
            if (pre_decoded == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                free(post_decoded_table);
                return NULL;
            }
        }

        if (!fill_pre_decoded(line, pre_decoded, pre_decoded_size, keyword_table, command_name, label_table, line_counter)) { /*Fill the pre-decoded array*/
            return NULL;
        }
        print_pre_decoded(pre_decoded, pre_decoded_size); /*tmp- to see predocded*/

        post_decoded = decode_pre_decoded(pre_decoded, pre_decoded_size, keyword_table, command_name, label_table); /*the array after decoding in decimal integers*/
        if (post_decoded != NULL) {
            post_decoded_table_size++;

            post_decoded_table = (int **)realloc(post_decoded_table, post_decoded_table_size + 1); /*1 for flag-NULL*/
            if (post_decoded_table == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                free(post_decoded_table);
                return NULL;
            }

            post_decoded_table[post_decoded_table_size - 1] = post_decoded;
            post_decoded_table[post_decoded_table_size] = NULL;
        }
    }

    printf("line counter is: %d\n", line_counter);
    free(pre_decoded);
    fclose(file); /* Close the file */

    print_2D_array(post_decoded_table);

    return post_decoded_table;
}

int deciphering_command(char *line, label *label_table, keyword *keyword_table, int current_line) {
    int i;
    char *line_from_first_letter = NULL;
    char commend_name[MAX_LINE_LENGTH];

    line_from_first_letter = pointer_after_label(line, label_table, current_line);
    remove_prefix_spaces(line_from_first_letter); /*skip unnecessary spaces*/


    for (i = 0; line_from_first_letter[i] != '\n' && !isspace(line_from_first_letter[i]); i++) {
        commend_name[i] = line_from_first_letter[i];
    }
    commend_name[i] = '\0';

    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (!strncmp(commend_name, keyword_table[i].name, strlen(keyword_table[i].name))) {
            if (strlen(commend_name) > strlen(keyword_table[i].name)) {
                return UNDEFINED;
            }
            return keyword_table[i].key;
        }
    }
    return UNDEFINED;
}

int arguments_amount_for_command(int command_name) {
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

int fill_pre_decoded(char *line, string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int command_name_key, label *label_table, int current_line) {
    int command_number;
    char *line_from_first_letter = NULL;

    /*adding the first argument- the command (in array[0])*/
    command_number = command_number_by_key(keyword_table, command_name_key);
    strcpy(pre_decoded[0].data, keyword_table[command_number].name);

    /*update the line pointer from the rellevant cell*/
    line_from_first_letter = pointer_after_label(line, label_table, current_line); /*skip the label (if it there) and spaces*/
    line_from_first_letter = &line_from_first_letter[keyword_table[command_number].length]; /*skip the command*/
    remove_prefix_spaces(line_from_first_letter); /*skip unnecessary spaces*/

    if (line_from_first_letter[0] == ',') { /*unnecessary comma between command and arguments*/
        printf("ERROR- An unnecessary comma between command and arguments\n");
        return FALSE;
    }

    if (pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS) { /* keep all the arguments in array[1] */
        return fill_pre_decoded_unknown_arguments_amount(line_from_first_letter, command_number, keyword_table, pre_decoded, label_table);
    }
    return fill_pre_decoded_known_arguments_amount(line_from_first_letter, pre_decoded, pre_decoded_size, label_table);
}

int fill_pre_decoded_unknown_arguments_amount(char *line, int command_number, keyword *keyword_table, string *pre_decoded, label *label_table) {
    int i, j, comma, end_of_argument_by_space, end_of_argument, end_of_string, last_quatiotion_mark, first_quatiotion_mark, flag;

    comma = end_of_argument_by_space = end_of_argument = end_of_string = first_quatiotion_mark = last_quatiotion_mark = flag = FALSE;

    for (i = 0, j = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        /* in case of .data */
        if (!strcmp(keyword_table[command_number].name, ".data")) {
            /*We need the data look like: int comma int comma int comma etc*/
            if (isspace(line[i]) && comma == FALSE) { /*we collected a full number that overed with space*/
                end_of_argument_by_space = TRUE;
            }
            remove_prefix_spaces(&line[i]);
            /*check wether the first char is a comma or not*/
            if (line[0] == ',') {
                printf("ERROR- Additional unnecessary comma before '.data' data\n");
                return FALSE;
            }
            /*in case of invalid symbol*/
            if (!(isdigit(line[i]) || line[i] == '-' || line[i] == '+' || line[i] == ',' || line[i] == '\0' || line[i] == '\n')) {
                printf("ERROR- Invalid symbol in '.data' data\n");
                return FALSE;
            }
            /*in case of a new number without a comma between the current and the last one*/
            if (end_of_argument_by_space == TRUE && line[i] != ',' && line[i] != '\0' && line[i] != '\n') { /*its a valid symbol*/
                printf("ERROR- NO COMMA BETWEEN INTEGERS\n");
                return FALSE;
            }
            /*in case of a series of commas without a number between them*/
            if (comma == TRUE && line[i] == ',') {
                printf("ERROR- A series of commas without a number between them\n");
                return FALSE;
            }

            /*The '.data' data is valid*/
            pre_decoded[1].data[j++] = line[i];
            pre_decoded[1].data[j] = '\0';

            /* upadte data for next loop */
            if (line[i] == ',') {
                comma = TRUE;
            }
            else {
                end_of_argument_by_space = comma = FALSE;
            }

            /*in case of problems after data collected*/
            if (line[i] == '\n' || line[i + 1] == '\n' || line[i] == '\0' || line[i + 1] == '\0') {
                if (comma == TRUE) {  /*in case of comma after the last number*/
                    printf("ERROR- An unnecessary comma in the end of '.data' data\n");
                    return FALSE;
                }
                if (!check_dotData_numbers(pre_decoded[1].data)) {
                    printf("ERROR- not a vaild integer\n");
                    return FALSE;
                }
            }
        }

        /* in case of ".string" */
        else if (!strcmp(keyword_table[command_number].name, ".string")) {
            if (i == 0) {
                if (line[0] == '\"') { /* The first data of ".string" is " */
                    first_quatiotion_mark = TRUE;
                }
                continue;
            }
            if (line[i] == '\"') {
                last_quatiotion_mark = TRUE;
            }
            else {
                last_quatiotion_mark = FALSE;
            }
            pre_decoded[1].data[j++] = line[i];
            pre_decoded[1].data[j] = '\0';

            if (line[i + 1] == '\n' || line[i + 1] == '\0') {
                if (first_quatiotion_mark == FALSE && last_quatiotion_mark == FALSE) { /* there are no " " */
                    printf("ERROR- There are no quatiotion marks at the the data in .string command\n");
                    fflush(stdout);
                    return FALSE;
                }
                else if (first_quatiotion_mark == FALSE && last_quatiotion_mark == TRUE) {
                    printf("ERROR- There is no \" at the beginning of the data in .string command\n");
                    fflush(stdout);
                    return FALSE;
                }
                else if (first_quatiotion_mark == TRUE && last_quatiotion_mark == FALSE) {
                    printf("ERROR- There is no \" at the end of the data in .string command\n");
                    fflush(stdout);
                    return FALSE;
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
                return FALSE;
            }
            if (end_of_argument == TRUE && (!isspace(line[i]))) {
                printf("ERROR- Too many arguments");
                return FALSE;
            }
            if (isspace(line[i]) || line[i + 1] == '\0' || line[i + 1] == '\n') {
                end_of_argument = TRUE;
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
        for (i = 0; i < label_table[0].size; i++) {
            if (!strcmp(pre_decoded[1].data, label_table[i].name)) {
                if (!strcmp(keyword_table[command_number].name, ".entry")) {
                    if (label_table[i].entry_or_extern == EXTERN) {
                        printf("ERROR - DEFINED A LABEL BY ENTRY AND EXTERN");
                        return FALSE;
                    }
                    label_table[i].entry_or_extern = ENTRY;
                }
                else {/*extern*/
                    if (label_table[i].entry_or_extern == ENTRY) {
                        printf("ERROR - DEFINED A LABEL BY ENTRY AND EXTERN");
                        return FALSE;
                    }
                    label_table[i].entry_or_extern = EXTERN;
                }
                flag = TRUE;
            }
        }
        if (flag == FALSE) {
            printf("ERROR- No such a label name");
            return FALSE;
        }
    }
    return TRUE;
}

int fill_pre_decoded_known_arguments_amount(char *line, string *pre_decoded, int pre_decoded_size, label *label_table) {
    int i, j, k, comma;

    comma = FALSE;

    for (i = 1; i < pre_decoded_size; i++) { /*loop for every argument*/
        j = 0;
        /*reached the end of the line without adding enough arguments*/
        if ((line[j] == '\0' || line[j] == '\n')) {
            printf("ERROR- There are less arguments than expected for the specific command\n");
            return FALSE;
        }

        /*fill argument in the right place*/
        for (k = 0; line[j] != '\0' && line[j] != '\n' && !isspace(line[j]) && line[j] != ','; j++) {
            pre_decoded[i].data[k++] = line[j];
            comma = FALSE;
        }
        pre_decoded[i].data[k] = '\0'; /*null terminator*/

        /*update to check errors between arguments*/
        line = &line[j];
        remove_prefix_spaces(line);

        /*a series of commas*/
        if (line[0] == ',' && comma == TRUE) {
            printf("ERROR- A series of commas without a number between them\n");
            return FALSE;
        }

        /*check about the comma between arguments and update line for the next loop*/
        if (line[0] == ',') {
            comma = TRUE;
            line = &line[1];
            remove_prefix_spaces(line);
        }
        else if (i != pre_decoded_size - 1) {
            printf("ERROR- There is no comma between arguments\n");
            return FALSE;
        }
        if (!known_argument_data_is_valid(pre_decoded[i].data, label_table)) {
            printf("ERROR- The argument data is not valid");
            return FALSE;
        }
    }

    for (i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        if (!isspace(line[i])) {
            printf("ERROR- Too many arguments for the specific command\n");
            return FALSE;
        }
    }

    if (comma == TRUE) {
        printf("ERROR- There is a comma after the last argument\n");
        return FALSE;
    }

    return TRUE;
}

int check_dotData_numbers(char *str) {
    int i, minus_or_plus, comma, number;
    minus_or_plus = comma = number = FALSE;

    for (i = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        if (str[i] == '-' || str[i] == '+') {
            if (number == TRUE || minus_or_plus == TRUE) { /*in case of 1,2,3-,4 or 1,+-2,3,4*/
                return FALSE;
            }
            minus_or_plus = TRUE;
            comma = number = FALSE;
        }
        else if (str[i] == ',') {
            if (minus_or_plus == TRUE) { /*in case of 1,-,2,3 */
                return FALSE;
            }
            comma = TRUE;
            number = minus_or_plus = FALSE;
        }
        else {   /*line[i] == number*/
            number = TRUE;
            comma = minus_or_plus = FALSE;
        }
    }
    return TRUE;
}

int known_argument_data_is_valid(char *str, label *label_table) {
    int tmp;

    tmp = map_addressing_method(str, label_table);

    if (tmp == 0 || tmp == 1 || tmp == 2 || tmp == 3) {
        return TRUE;
    }
    return FALSE;
}
