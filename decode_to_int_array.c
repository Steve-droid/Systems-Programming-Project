#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decode_to_int_array.h"
#include "decoding.h"
#include "label_table.h"
#include "decode_to_string_array.h"


int *decode_pre_decoded(string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int key, label *label_table) {
    int command_val, post_decoded_size;
    int *post_decoded = NULL;

    command_val = decode_pre_decoded_command(pre_decoded, pre_decoded_size, keyword_table, key, label_table);

    if (command_val == NO_NEED_TO_DECODE) {
        return NULL;
    }

    post_decoded_size = 1;
    post_decoded = (int *)malloc((post_decoded_size + 1) * sizeof(int)); /*1 for flag*/
    if (post_decoded == NULL) {
        printf("ERROR- allocation failed");
        return NULL;
    }

    post_decoded[0] = command_val;
    post_decoded[1] = FLAG;

    /* commands without data */
    if (JSR == command_number_by_key(keyword_table, key) || STOP == command_number_by_key(keyword_table, key)) {
        return post_decoded;
    }
    post_decoded = decode_pre_decoded_data(command_val, post_decoded, pre_decoded, pre_decoded_size, label_table);

    return post_decoded;
}

int decode_pre_decoded_command(string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int key, label *label_table) {
    int opcode, i, addressing_method, val;
    int *binary_command = NULL;

    opcode = decode_pre_decoded_command_opcode(keyword_table, key);
    if (opcode == NO_NEED_TO_DECODE) {
        return NO_NEED_TO_DECODE;
    }
    if (opcode == UNKNOWN_NUMBER_OF_ARGUMENTS) {
        return UNKNOWN_NUMBER_OF_ARGUMENTS;
    }

    binary_command = (int *)malloc(OUTPUT_COMMAND_LEN * sizeof(int));
    if (binary_command == NULL) {
        printf("ERROR- ALOOCATION FAILED\n");
        return UNDEFINED;
    }
    /*initillize command*/
    for (i = 0; i < OUTPUT_COMMAND_LEN; i++) {
        binary_command[i] = 0;
    }

    /* fill the opcode */
    int_to_binary_array(opcode, binary_command, 0, 3);

    for (i = 1, addressing_method = 0; i < pre_decoded_size; i++) {
        addressing_method = map_addressing_method(pre_decoded[i].data, label_table);

        /* clr, not, inc, dec, jmp, bne, red, prn, jsr */
        if (pre_decoded_size == 2) { /*destination*/
            binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + 3 - addressing_method] = 1;
        }
        else { /* mov, cmp, add, sub, lea */
            if (i == 1) { /*source */
                binary_command[OPCODE_LEN + 3 - addressing_method] = 1;

            }
            else { /*destination*/
                binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + 3 - addressing_method] = 1;
            }
        }
    }

    binary_command[A] = 1;

    if (!addressing_methods_are_legal(binary_command, keyword_table, key)) {
        printf("ERROR- The addressing method does not match the command");
        free(binary_command);
        return UNDEFINED;
    }
    val = binary_array_to_int(binary_command);
    free(binary_command);
    return val;

}

int decode_pre_decoded_command_opcode(keyword *keyword_table, int key) {
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

int addressing_methods_are_legal(int *binary_command, keyword *keyword_table, int key) {
    int decision_source, decision_destination;

    decision_source = addressing_methods_are_legal_source(binary_command, keyword_table, key);
    decision_destination = addressing_methods_are_legal_destination(binary_command, keyword_table, key);

    return decision_source && decision_destination;
}

int addressing_methods_are_legal_source(int *binary_command, keyword *keyword_table, int key) {
    int decision, i;

    switch (command_number_by_key(keyword_table, key)) {
        /* case 0 , 1 , 2 , 3 */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
        decision = FALSE;
        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = TRUE;
            }
        }
        break;
        /* case 1 */
    case LEA:
        if (binary_command[OPCODE_LEN + (3 - 1)] != 1) { /*addresing method 1*/
            return FALSE;
        }
        decision = TRUE;

        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if (i != OPCODE_LEN + (3 - 1) && binary_command[i] == 1) {
                decision = FALSE;
            }
        }
        break;
        /* case - no source */
    default: /* clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop */
        decision = TRUE;
        for (i = OPCODE_LEN; i < OPCODE_LEN + ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = FALSE;
            }
        }
        break;
    }
    return decision;
}

int addressing_methods_are_legal_destination(int *binary_command, keyword *keyword_table, int key) {

    int decision, i;

    switch (command_number_by_key(keyword_table, key)) {
        /* case 0 , 1 , 2 , 3 */
    case CMP:
    case PRN:
        decision = FALSE;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = TRUE;
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
            return FALSE;
        }
        decision = FALSE;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (i != OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0) && binary_command[i] == 1) {
                decision = TRUE;
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
            return FALSE;
        }
        decision = TRUE;
        /*in case of 0 or 3 is on*/
        if (binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 0)] == 1 ||
            binary_command[OPCODE_LEN + ADDRESSING_METHOD_LEN + (3 - 3)] == 1) {
            decision = FALSE;
        }

        break;
        /* case no destination */
    default: /* rts , stop */
        decision = TRUE;
        for (i = OPCODE_LEN + ADDRESSING_METHOD_LEN; i < OPCODE_LEN + 2 * ADDRESSING_METHOD_LEN; i++) {
            if (binary_command[i] == 1) {
                decision = FALSE;
            }
        }
        break;
    }
    return decision;
}

int *decode_pre_decoded_data(int which_data, int *post_decoded, string *pre_decoded, int pre_decoded_size, label *label_table) {
    if (which_data == UNKNOWN_NUMBER_OF_ARGUMENTS) {
        return decode_pre_decoded_data_unknown_arguments_amount(pre_decoded);
    }
    return decode_pre_decoded_data_known_arguments_amount(pre_decoded, pre_decoded_size, post_decoded, label_table);
}

int *decode_pre_decoded_data_unknown_arguments_amount(string *pre_decoded) {
    int i, data_arr_size;
    int *data_arr = NULL;

    if (!strcmp(pre_decoded[0].data, ".data")) {
        data_arr = convert_to_int_array(pre_decoded[1].data);
    }
    else { /*string - need the ascii val*/
        data_arr_size = 0;
        data_arr = (int *)malloc((data_arr_size + 1) * sizeof(int *)); /* 1 for ending array flag*/
        if (data_arr == NULL) {
            printf("ERROR-ALOOCATION FAILED");
            return NULL;
        }
        data_arr[0] = FLAG;

        for (i = 0; pre_decoded[1].data[i] != '\0'; i++) {
            data_arr_size++;
            data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
            if (data_arr == NULL) {
                printf("ERROR-ALOOCATION FAILED");
                return NULL;
            }
            data_arr[data_arr_size - 1] = pre_decoded[1].data[i];
            data_arr[data_arr_size] = FLAG;

        }
        /*last cell - terminator*/
        data_arr_size++;
        data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
        if (data_arr == NULL) {
            printf("ERROR-ALOOCATION FAILED");
            return NULL;
        }
        data_arr[data_arr_size - 1] = 0; /*String terminator*/
        data_arr[data_arr_size] = FLAG;

    }

    return data_arr;
}

int *decode_pre_decoded_data_known_arguments_amount(string *pre_decoded, int pre_decoded_size, int *post_decoded, label *label_table) {
    int i, addressing_method, addressing_method_argument_1, addressing_method_argument_2, post_decoded_size;

    post_decoded_size = 1;


    /*in case of 2 registers in data*/
    if (pre_decoded_size == 3) {

        addressing_method_argument_1 = map_addressing_method(pre_decoded[1].data, label_table);
        addressing_method_argument_2 = map_addressing_method(pre_decoded[2].data, label_table);

        if ((addressing_method_argument_1 == 2 || addressing_method_argument_1 == 3) &&
            (addressing_method_argument_2 == 2 || addressing_method_argument_2 == 3)) {
            post_decoded_size = 2;
            post_decoded = (int *)realloc(post_decoded, (post_decoded_size + 1) * sizeof(int *)); /* 1 for flag of end of array*/
            if (post_decoded == NULL) {
                printf("ERROR - allocation failed");
                return NULL;
            }
            post_decoded[1] = decode_2_registers_command(pre_decoded[1].data, pre_decoded[2].data);
            post_decoded[2] = FLAG;
            return post_decoded;
        }
    }

    /*in case of every other option*/
    for (i = 1; i < pre_decoded_size; i++) {
        post_decoded_size++;
        post_decoded = (int *)realloc(post_decoded, (post_decoded_size + 1) * sizeof(int));
        if (post_decoded == NULL) {
            printf("ERROR- ALLOCATION FAILED");
            return NULL;
        }

        addressing_method = map_addressing_method(pre_decoded[i].data, label_table);

        /*in case of one argument - the argument is destination*/
        if (pre_decoded_size == 2) {
            post_decoded[i] = map_argument_data(pre_decoded[i].data, addressing_method, label_table, DEST);
            post_decoded[i + 1] = FLAG;
        }
        else {
            if (i == 1) {
                post_decoded[i] = map_argument_data(pre_decoded[i].data, addressing_method, label_table, SOURCE);
            }
            else {
                post_decoded[i] = map_argument_data(pre_decoded[i].data, addressing_method, label_table, DEST);
                post_decoded[i + 1] = FLAG;
            }
        }
    }

    return post_decoded;

}

int decode_2_registers_command(char *source, char *dest) {
    int val, source_reg, dest_reg;

    source_reg = which_register(source);
    dest_reg = which_register(dest);

    val = 0;
    val += source_reg << 7;
    val += dest_reg << 3;
    val += 4; /* A */
    return val;
}

int map_argument_data(char *str, int addressing_method, label *label_table, int source_or_dest) {
    int label_location, val;

    val = 0;

    switch (addressing_method) {
    case 0: /*return the number after '#'  */
        val = atoi(&str[1]) << 3;
        val += 4; /*4 for A*/
        break;
    case 1: /*return the key for each label to map their address later*/
        label_location = string_to_label(str, label_table);
        val = label_table[label_location].key;
        break;
    case 2: /* return the number after '*r'  */
        val = calc_methods_2_3(atoi(&str[2]), source_or_dest);
        break;
    default: /* case 3:  return the number after 'r'  */
        val = calc_methods_2_3(atoi(&str[1]), source_or_dest);
        break;
    }
    return val;
}

int calc_methods_2_3(int number, int pos) {
    int result = 0;

    /* If the position is SOURCE, place the number in bits 7-10 */
    if (pos == SOURCE) {
        result |= (number & 0x0F) << 7;
    }
    /* If the position is DEST, place the number in bits 3-6 */
    else if (pos == DEST) {
        result |= (number & 0x0F) << 3;
    }

    /* Set bit 2 to 1 */
    result |= 1 << 2;

    return result;
}

int which_register(char *reg) {
    int i;

    for (i = 0; reg[i] != 'r' && reg[i] != '\0'; i++);

    return atoi(&reg[i + 1]);
}