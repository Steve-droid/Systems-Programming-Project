#include "codegen.h"

#define WORD_LENGTH 15

/*******delete later*******/
typedef struct binary_word {
    int bits[WORD_LENGTH];
}binary_word;

typedef struct data_section {
    size_t DC;
    binary_word **data;
}data_section;

typedef struct instruction_section {
    size_t IC;
    binary_word **instructions;
}instruction_section;
/*******delete later*******/

int convert_instruction_to_binary(string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int key, label_table *_label_table) {
    int opcode, i, addressing_method, val;
    int *binary_command = NULL;

    opcode = get_command_opcode(keyword_table, key);
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
        addressing_method = get_addressing_method(pre_decoded[i].data, _label_table);

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

    if (!validate_addressing_methods(binary_command, keyword_table, key)) {
        printf("ERROR- The addressing method does not match the command");
        free(binary_command);
        return UNDEFINED;
    }
    val = binary_array_to_int(binary_command);
    free(binary_command);
    return val;

}


int *convert_pre_lexed_fields_to_binary(string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int key, label_table *_label_table) {
    int command_val, post_decoded_size;
    int *post_decoded = NULL;

    command_val = convert_instruction_to_binary(pre_decoded, pre_decoded_size, keyword_table, key, _label_table);

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
    post_decoded = convert_data_fields_to_binary(command_val, post_decoded, pre_decoded, pre_decoded_size, _label_table);

    return post_decoded;
}

int *convert_data_fields_to_binary(int which_data, int *post_decoded, string *pre_decoded, int pre_decoded_size, label_table *_label_table) {
    if (which_data == UNKNOWN_NUMBER_OF_ARGUMENTS) {
        return convert_data_fields_to_binary_unknown_args(pre_decoded);
    }
    return convert_data_fields_to_binary_known_args(pre_decoded, pre_decoded_size, post_decoded, _label_table);
}

int *convert_data_fields_to_binary_unknown_args(string *pre_decoded) {
    int i, data_arr_size;
    int *data_arr = NULL;

    if (!strcmp(pre_decoded[0].data, ".data")) {
        data_arr = convert_to_int_array(pre_decoded[1].data);
    }
    else { /*string - need the ascii val*/
        data_arr_size = 0;
        data_arr = (int *)malloc((data_arr_size + 1) * sizeof(int *)); /* 1 for ending array flag*/
        if (data_arr == NULL) {
            printf("ERROR- ALLOCATION FAILED");
            return NULL;
        }
        data_arr[0] = FLAG;

        for (i = 0; pre_decoded[1].data[i] != '\0'; i++) {
            data_arr_size++;
            data_arr = (int *)realloc(data_arr, (data_arr_size + 1) * sizeof(int)); /* 1 for ending array flag*/
            if (data_arr == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                return NULL;
            }
            data_arr[data_arr_size - 1] = pre_decoded[1].data[i];
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


int *convert_data_fields_to_binary_known_args(string *pre_decoded, int pre_decoded_size, int *post_decoded, label_table *_label_table) {
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

int convert_two_registers_to_binary(char *source, char *dest) {
    int val, source_reg, dest_reg;

    source_reg = get_register_number(source);
    dest_reg = get_register_number(dest);

    val = 0;
    val += source_reg << 7;
    val += dest_reg << 3;
    val += 4; /* A */
    return val;
}



int convert_argument_to_binary(char *str, int addressing_method, label_table *_label_table, int source_or_dest) {
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

int calculate_addressing_method_bits(int number, int pos) {
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


/**
*@brief Create a .ob (i.e object) file for every filename
* The .object file is contains the binary representation of the instructions and data
* The first line in the .ob file consists of 2 fields:
* 1. The number of instruction words in the object file- stored in IC
* 2. The number of data words in the object file- stored in DC
* The following lines contain 2 fields:
* 1. The address of the word in the memory- in decimal
* 2. The word itself- in decimal
* @param inst_section A struct containing the instruction section and the IC
* @param data__section A struct containing the data section and the DC
* @param generic_filename A generic filename to create the object file
* @param num_of_files The number of files to create
*/
status create_object_file(instruction_section inst_section, data_section data__section, char *generic_filename) {
    char *object_filename = NULL;
    FILE *object_file = NULL;
    size_t i;

    object_filename = create_file_name(generic_filename, ".ob");

    object_file = fopen(object_filename, "w");
    if (object_file == NULL) {
        printf("Error: Could not create object file %s\n", object_filename);
        return STATUS_ERROR_WRITE;
    }

    fprintf(object_file, "  Instruction count: %ld Data count: %ld\n", inst_section.IC, data__section.DC);

    for (i = 0; i < inst_section.IC; i++) {
        fprintf(object_file, "%ld %d\n", i, binary_array_to_int(inst_section.instructions[i]->bits));
    }

    for (i = 0; i < data__section.DC; i++) {
        fprintf(object_file, "%ld %d\n", i + inst_section.IC, binary_array_to_int(data__section.data[i]->bits));
    }



    return STATUS_OK;

}