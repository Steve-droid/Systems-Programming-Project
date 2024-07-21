#include "parser.h"
#define FIRST_FIELD 0
#define SECOND_FIELD 1
#define THIRD_FIELD 2
#define FOURTH_FIELD 3
#define FIRST_ARG 0
#define SECOND_ARG 1
#define FIRST_TOKEN 0
#define SECOND_TOKEN 1
#define THIRD_TOKEN 2
#define DONT_CONVERT -10
#define OPCODE_FIRST_BIT 0
#define OPCODE_LAST_BIT 3
#define WORD_LENGTH 15
#define INDEX_A 12

// static status convert_known_amount(string *pre_decoded, int pre_decoded_size, int *post_decoded, label_table *_label_table) {
//     int i, addressing_method, addressing_method_argument_1, addressing_method_argument_2, post_decoded_size;

//     post_decoded_size = 1;


//     /*in case of 2 registers in data*/
//     if (pre_decoded_size == 3) {

//         addressing_method_argument_1 = get_addressing_method(pre_decoded[1].data, _label_table);
//         addressing_method_argument_2 = get_addressing_method(pre_decoded[2].data, _label_table);

//         if ((addressing_method_argument_1 == 2 || addressing_method_argument_1 == 3) &&
//             (addressing_method_argument_2 == 2 || addressing_method_argument_2 == 3)) {
//             post_decoded_size = 2;
//             post_decoded = (int *)realloc(post_decoded, (post_decoded_size + 1) * sizeof(int *)); /* 1 for flag of end of array*/
//             if (post_decoded == NULL) {
//                 printf("ERROR- ALLOCATION FAILED");
//                 return NULL;
//             }
//             post_decoded[1] = convert_two_registers_to_binary(pre_decoded[1].data, pre_decoded[2].data);
//             post_decoded[2] = FLAG;
//             return post_decoded;
//         }
//     }

//     /*in case of every other option*/
//     for (i = 1; i < pre_decoded_size; i++) {
//         post_decoded_size++;
//         post_decoded = (int *)realloc(post_decoded, (post_decoded_size + 1) * sizeof(int));
//         if (post_decoded == NULL) {
//             printf("ERROR- ALLOCATION FAILED");
//             return NULL;
//         }

//         addressing_method = get_addressing_method(pre_decoded[i].data, _label_table);

//         /*in case of one argument - the argument is destination*/
//         if (pre_decoded_size == 2) {
//             post_decoded[i] = convert_argument_to_binary(pre_decoded[i].data, addressing_method, _label_table, DEST);
//             post_decoded[i + 1] = FLAG;
//         }
//         else {
//             if (i == 1) {
//                 post_decoded[i] = convert_argument_to_binary(pre_decoded[i].data, addressing_method, _label_table, SOURCE);
//             }
//             else {
//                 post_decoded[i] = convert_argument_to_binary(pre_decoded[i].data, addressing_method, _label_table, DEST);
//                 post_decoded[i + 1] = FLAG;
//             }
//         }
//     }

//     return post_decoded;
// }


// static bin_word *get_first_word(bin_word *first_word, inst *_inst, keyword *keyword_table, label_table *_label_table) {
//     /* binary_command == first_word */
//     int opcode = UNDEFINED;
//     size_t i;
//     int addressing_method = UNDEFINED;
//     size_t bit_to_turn_on = 0;
//     int val = UNDEFINED;
//     bin_inst *_binary_inst = NULL;

//     create_bin_inst(_binary_inst);

//     opcode = get_command_opcode(keyword_table, _inst->cmd_key);

//     if (opcode == DONT_CONVERT) return DONT_CONVERT;

//     if (opcode == UNKNOWN_NUMBER_OF_ARGUMENTS) return UNKNOWN_NUMBER_OF_ARGUMENTS;

//     /* fill the opcode */
//     int_to_binary_array(opcode, first_word, OPCODE_FIRST_BIT, OPCODE_LAST_BIT);

//     for (i = 1, addressing_method = 0; i < _inst->num_tokens; i++) {
//         addressing_method = get_addressing_method(_inst->tokens[i], _label_table);

//         /* clr, not, inc, dec, jmp, bne, red, prn, jsr */
//         if (_inst->num_tokens == 2) { /*destination*/
//             bit_to_turn_on = OPCODE_LEN + ADDRESSING_METHOD_LEN + 3 - addressing_method;
//             first_word->bits_vec[bit_to_turn_on] = 1;
//         }
//         else { /* mov, cmp, add, sub, lea */
//             if (i == 1) { /*source */
//                 bit_to_turn_on = OPCODE_LEN + 3 - addressing_method;
//                 first_word->bits_vec[bit_to_turn_on] = 1;

//             }
//             else { /*destination*/
//                 bit_to_turn_on = OPCODE_LEN + ADDRESSING_METHOD_LEN + 3 - addressing_method;
//                 first_word->bits_vec[bit_to_turn_on] = 1;
//             }
//         }
//     }

//     first_word->bits_vec[INDEX_A] = 1;

//     if (!validate_addressing_methods(first_word, keyword_table, _inst->cmd_key)) {
//         printf("ERROR- The addressing method does not match the command");
//         destroy_bin_inst(_binary_inst);
//         return UNDEFINED;
//     }

//     return first_word;

// }

// static status *convert_args(bool arg_amount_is_known, bin_inst *_binary_inst, inst *_inst, label_table *_label_table) {
//     if (arg_amount_is_known) return convert_known_args(_binary_inst, _inst, _label_table);
//     else return convert_known_amount(_binary_inst, _inst, _label_table);
// }

// static status convert_unknown_amount(bin_inst *_binary_inst, inst *_inst, label_table *_label_table) {
//     int i, data_arr_size;
//     int *data_arr = NULL;

//     if (!strcmp(_inst->tokens[0], ".data")) {
//         data_arr = convert_to_int_array(_inst->tokens[1]);
//     }
//     else { /*string - need the ascii val*/
//         data_arr_size = 0;
//         data_arr = (int *)malloc((data_arr_size + 1) * sizeof(int *)); /* 1 for ending array flag*/
//         if (data_arr == NULL) {
//             printf("ERROR- ALLOCATION FAILED");
//             return NULL;
//         }
//         data_arr[0] = FLAG;

//         for (i = 0; _inst->tokens[1][i] != '\0'; i++) {
//             data_arr_size++;
//             data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
//             if (data_arr == NULL) {
//                 printf("ERROR- ALLOCATION FAILED");
//                 return NULL;
//             }
//             data_arr[data_arr_size - 1] = _inst->tokens[1][i];
//             data_arr[data_arr_size] = FLAG;

//         }
//         /*last cell - terminator*/
//         data_arr_size++;
//         data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
//         if (data_arr == NULL) {
//             printf("ERROR- ALLOCATION FAILED");
//             return NULL;
//         }
//         data_arr[data_arr_size - 1] = 0; /*String terminator*/
//         data_arr[data_arr_size] = FLAG;

//     }

//     return data_arr;
// }

// static status convert_registers(char *source, char *dest) {
//     int val, source_reg, dest_reg;

//     source_reg = get_register_number(source);
//     dest_reg = get_register_number(dest);

//     val = 0;
//     val += source_reg << 7;
//     val += dest_reg << 3;
//     val += 4; /* A */
//     return val;
// }

// static status convert_arg_addressing(char *str, int addressing_method, label_table *_label_table, int source_or_dest) {
//     int val = 0;
//     label *_label = NULL;

//     val = 0;

//     switch (addressing_method) {
//     case 0: /*return the number after '#'  */
//         val = atoi(&str[1]) << 3;
//         val += 4; /*4 for A*/
//         break;
//     case 1: /*return the key for each label to map their address later*/
//         _label = get_label(_label_table, str);

//         if (_label == NULL) {
//             printf("ERROR- label not found");
//             return UNDEFINED;
//         }
//         val = _label->key;
//         break;
//     case 2: /* return the number after '*r'  */
//         val = calculate_addressing_method_bits(atoi(&str[2]), source_or_dest);
//         break;
//     default: /* case 3:  return the number after 'r'  */
//         val = calculate_addressing_method_bits(atoi(&str[1]), source_or_dest);
//         break;
//     }
//     return val;
// }

// static status calculate_addressing_method_bits(int number, int pos) {
//     int result = 0;

//     /* If the position is SOURCE, place the number in bits 7-10 */
//     if (pos == SOURCE) {
//         result |= (number & 0x0F) << 7;
//     }
//     /* If the position is DEST, place the number in bits 3-6 */
//     else if (pos == DEST) {
//         result |= (number & 0x0F) << 3;
//     }

//     /* Set bit 2 to 1 */
//     result |= 1 << 2;

//     return result;
// }

static status generate_binary_words(inst *_inst, bin_table *_binary_table, keyword *keyword_table, label_table *_label_table) {
    /* Post decoded == binary instruction table  */
    bin_word *first_word = NULL;
    bin_inst *_binary_inst = NULL;
    bool arg_amount_is_known = false;
    status conv_status = STATUS_ERROR;

    if (_inst == NULL || _binary_table == NULL || keyword_table == NULL || _label_table == NULL) {
        printf("Error- Received NULL pointer\n");
        return STATUS_ERROR;
    }

    if (create_bin_inst(_binary_table) == STATUS_ERROR) {
        printf("ERROR- Failed to create a binary instruction");
        return STATUS_ERROR;
    }


    if (first_word == NULL) {
        printf("ERROR- Failed to convert the first word to binary");
        return STATUS_ERROR;
    }

    /* Insert the first word to the binary instruction */
    insert_bin_word_to_inst(_binary_inst, first_word);

    /* Insert the binary instruction to the binary table */
    insert_bin_inst_to_table(_binary_table, _binary_inst);

    /* commands without data */
    if (JSR == command_number_by_key(keyword_table, _inst->cmd_key) || STOP == command_number_by_key(keyword_table, _inst->cmd_key)) {
        return STATUS_OK;
    }

    return STATUS_OK;

}


// /*fill_label_table_addresses*/
// void assign_label_addresses(int **decoded_table, label_table *_label_table) {
//     int i, j, k, counter;

//     for (k = 0; k < _label_table->size; k++) {
//         counter = FIRST_ADDRESS;
//         for (i = 0; decoded_table && decoded_table[i] != NULL && i < _label_table->labels[k]->instruction_line; i++) {
//             for (j = 0; decoded_table[i][j] != FLAG; j++) {
//                 counter++;
//             }
//         }
//         _label_table->labels[k]->address = counter;
//         _label_table->labels[k]->address = _label_table->labels[k]->address << 3;
//         _label_table->labels[k]->address += _label_table->labels[k]->is_extern ? 1 : 2;
//     }
// }

// /*decode_label_addresses*/
// void replace_label_keys_with_addresses(int *decoded, label_table *_label_table) {
//     int i, k;

//     for (k = 0; k < _label_table->size; k++) {
//         for (i = 0; decoded && decoded[i] != FLAG; i++) {
//             if (_label_table->labels[k]->key == decoded[i]) {
//                 decoded[i] = _label_table->labels[k]->address;
//             }
//         }
//     }
// }

// */




/**
 *@brief This functions gets an instruction table consisting of valid instructions as tokens and converts them to binary.
 *
 * @param am_filename
 * @param _label_table
 * @param keyword_table
 * @return bin_table*
 */
bin_table *parse(inst_table *_inst_table, label_table *_label_table, keyword *keyword_table) {
    bin_table *_bin_table = NULL;
    size_t i;
    status conv_status = STATUS_ERROR;
    inst *current_inst = NULL;

    if (create_bin_table(&_bin_table) == STATUS_ERROR) {
        printf("ERROR- Failed to create a binary table");
        return NULL;
    }

    for (i = 0; i < _inst_table->num_instructions; i++) {
        current_inst = _inst_table->inst_vec[i];
        conv_status = generate_binary_words(current_inst, _bin_table, keyword_table, _label_table);
        if (conv_status == STATUS_ERROR) {
            printf("ERROR- Failed to convert the instruction to binary");
            return NULL;
        }
    }

    return _bin_table;


}
