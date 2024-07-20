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

static status convert_known_amount(string *pre_decoded, int pre_decoded_size, int *post_decoded, label_table *_label_table) {
    int i, addressing_method, addressing_method_argument_1, addressing_method_argument_2, post_decoded_size;

    post_decoded_size = 1;


    /*in case of 2 registers in data*/
    if (pre_decoded_size == 3) {

        addressing_method_argument_1 = get_addressing_method(pre_decoded[1].data, _label_table);
        addressing_method_argument_2 = get_addressing_method(pre_decoded[2].data, _label_table);

        if ((addressing_method_argument_1 == 2 || addressing_method_argument_1 == 3) &&
            (addressing_method_argument_2 == 2 || addressing_method_argument_2 == 3)) {
            post_decoded_size = 2;
            post_decoded = (int *)realloc(post_decoded, (post_decoded_size + 1) * sizeof(int *)); /* 1 for flag of end of array*/
            if (post_decoded == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                return NULL;
            }
            post_decoded[1] = convert_two_registers_to_binary(pre_decoded[1].data, pre_decoded[2].data);
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

        addressing_method = get_addressing_method(pre_decoded[i].data, _label_table);

        /*in case of one argument - the argument is destination*/
        if (pre_decoded_size == 2) {
            post_decoded[i] = convert_argument_to_binary(pre_decoded[i].data, addressing_method, _label_table, DEST);
            post_decoded[i + 1] = FLAG;
        }
        else {
            if (i == 1) {
                post_decoded[i] = convert_argument_to_binary(pre_decoded[i].data, addressing_method, _label_table, SOURCE);
            }
            else {
                post_decoded[i] = convert_argument_to_binary(pre_decoded[i].data, addressing_method, _label_table, DEST);
                post_decoded[i + 1] = FLAG;
            }
        }
    }

    return post_decoded;
}

/*decode*/
int *proccess_assembly_code(char *am_filename, label_table *_label_table, keyword *keyword_table) {

    int *one_dimentional_instruction_table = NULL;
    int **instruction_table = NULL;

    instruction_table = lex(am_filename, _label_table, keyword_table);
    one_dimentional_instruction_table = convert_twodim_array_to_onedim(instruction_table);
    printf("\nINSTRUCTION TABLE- LABEL PRESENTED AS LABEL'S KEY:\n");
    print_array_in_binary(one_dimentional_instruction_table);

    assign_label_addresses(instruction_table, _label_table);
    one_dimentional_instruction_table = convert_twodim_array_to_onedim(instruction_table);
    replace_label_keys_with_addresses(one_dimentional_instruction_table, _label_table);

    free(instruction_table);
    return one_dimentional_instruction_table;
}

static bin_word *get_first_word(bin_word *first_word, inst *_inst, keyword *keyword_table, label_table *_label_table) {
    /* binary_command == first_word */
    int opcode = UNDEFINED;
    size_t i;
    int addressing_method = UNDEFINED;
    size_t bit_to_turn_on = 0;
    int val = UNDEFINED;
    bin_inst *_binary_inst = NULL;

    create_bin_inst(_binary_inst);

    opcode = get_command_opcode(keyword_table, _inst->cmd_key);

    if (opcode == DONT_CONVERT) return DONT_CONVERT;

    if (opcode == UNKNOWN_NUMBER_OF_ARGUMENTS) return UNKNOWN_NUMBER_OF_ARGUMENTS;

    /* fill the opcode */
    int_to_binary_array(opcode, first_word, OPCODE_FIRST_BIT, OPCODE_LAST_BIT);

    for (i = 1, addressing_method = 0; i < _inst->num_tokens; i++) {
        addressing_method = get_addressing_method(_inst->tokens[i], _label_table);

        /* clr, not, inc, dec, jmp, bne, red, prn, jsr */
        if (_inst->num_tokens == 2) { /*destination*/
            bit_to_turn_on = OPCODE_LEN + ADDRESSING_METHOD_LEN + 3 - addressing_method;
            first_word->bits_vec[bit_to_turn_on] = 1;
        }
        else { /* mov, cmp, add, sub, lea */
            if (i == 1) { /*source */
                bit_to_turn_on = OPCODE_LEN + 3 - addressing_method;
                first_word->bits_vec[bit_to_turn_on] = 1;

            }
            else { /*destination*/
                bit_to_turn_on = OPCODE_LEN + ADDRESSING_METHOD_LEN + 3 - addressing_method;
                first_word->bits_vec[bit_to_turn_on] = 1;
            }
        }
    }

    first_word->bits_vec[INDEX_A] = 1;

    if (!validate_addressing_methods(first_word, keyword_table, _inst->cmd_key)) {
        printf("ERROR- The addressing method does not match the command");
        destroy_bin_inst(_binary_inst);
        return UNDEFINED;
    }

    return first_word;

}


static status *convert_args(bool arg_amount_is_known, bin_inst *_binary_inst, inst *_inst, label_table *_label_table) {
    if (arg_amount_is_known) return convert_known_args(_binary_inst, _inst, _label_table);
    else return convert_known_amount(_binary_inst, _inst, _label_table);
}

static status convert_unknown_amount(bin_inst *_binary_inst, inst *_inst, label_table *_label_table) {
    int i, data_arr_size;
    int *data_arr = NULL;

    if (!strcmp(_inst->tokens[0], ".data")) {
        data_arr = convert_to_int_array(_inst->tokens[1]);
    }
    else { /*string - need the ascii val*/
        data_arr_size = 0;
        data_arr = (int *)malloc((data_arr_size + 1) * sizeof(int *)); /* 1 for ending array flag*/
        if (data_arr == NULL) {
            printf("ERROR- ALLOCATION FAILED");
            return NULL;
        }
        data_arr[0] = FLAG;

        for (i = 0; _inst->tokens[1][i] != '\0'; i++) {
            data_arr_size++;
            data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
            if (data_arr == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                return NULL;
            }
            data_arr[data_arr_size - 1] = _inst->tokens[1][i];
            data_arr[data_arr_size] = FLAG;

        }
        /*last cell - terminator*/
        data_arr_size++;
        data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
        if (data_arr == NULL) {
            printf("ERROR- ALLOCATION FAILED");
            return NULL;
        }
        data_arr[data_arr_size - 1] = 0; /*String terminator*/
        data_arr[data_arr_size] = FLAG;

    }

    return data_arr;
}



static status convert_registers(char *source, char *dest) {
    int val, source_reg, dest_reg;

    source_reg = get_register_number(source);
    dest_reg = get_register_number(dest);

    val = 0;
    val += source_reg << 7;
    val += dest_reg << 3;
    val += 4; /* A */
    return val;
}

static status convert_arg_addressing(char *str, int addressing_method, label_table *_label_table, int source_or_dest) {
    int val = 0;
    label *_label = NULL;

    val = 0;

    switch (addressing_method) {
    case 0: /*return the number after '#'  */
        val = atoi(&str[1]) << 3;
        val += 4; /*4 for A*/
        break;
    case 1: /*return the key for each label to map their address later*/
        _label = get_label(_label_table, str);

        if (_label == NULL) {
            printf("ERROR- label not found");
            return UNDEFINED;
        }
        val = _label->key;
        break;
    case 2: /* return the number after '*r'  */
        val = calculate_addressing_method_bits(atoi(&str[2]), source_or_dest);
        break;
    default: /* case 3:  return the number after 'r'  */
        val = calculate_addressing_method_bits(atoi(&str[1]), source_or_dest);
        break;
    }
    return val;
}

static status calculate_addressing_method_bits(int number, int pos) {
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

static status convert_inst_to_binary(inst *_inst, bin_inst *_binary_inst, bin_table *_binary_table, keyword *keyword_table, label_table *_label_table) {
    /* Post decoded == binary instruction table  */
    bin_word *first_word = NULL;
    bool arg_amount_is_known = false;
    status conv_status = STATUS_ERROR;


    init_word(first_word);
    init_inst(_binary_inst);
    init_table(_binary_table);

    first_word = get_first_word(first_word, _inst, keyword_table, _label_table);


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

    if (_inst->cmd_key == UNKNOWN_NUMBER_OF_ARGUMENTS) {
        arg_amount_is_known = false;
    }
    else {
        arg_amount_is_known = true;
    }

    conv_status = convert_args_to_binary(arg_amount_is_known, _binary_inst, _inst, _label_table);

    if (conv_status == STATUS_ERROR) {
        printf("ERROR- Failed to convert the arguments to binary");
        return STATUS_ERROR;
    }

    return STATUS_OK;

    // post_decoded = convert_data_fields_to_binary(command_val, post_decoded, pre_decoded, pre_decoded_size, _label_table);

}









