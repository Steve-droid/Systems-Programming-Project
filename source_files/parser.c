#include "parser.h"
#include <string.h>
#define ARE_SHIFT 0
#define DEST_SHIFT 3
#define SRC_SHIFT 7
#define OPCODE_SHIFT 11

uint16_t to_twos_complement(int16_t number) {
    return (uint16_t)number;
}

static void assign_bits_directive(inst_table *_inst_table, size_t index) {
    char *temp = NULL;
    char *token = NULL;
    int i = 0;
    inst *_inst = _inst_table->inst_vec[index];


    if (_inst->is_dot_data) {
        temp = strdup(_inst->tokens[1]);
        token = strtok(temp, ",");

        while (token != NULL && i < _inst->num_dot_data_members) {
            _inst->binary_word_vec[i] = to_twos_complement(atoi(token));
            token = strtok(NULL, ", ");
            i++;
        }

        free(temp);
        temp = NULL;

        return;
    }

    if (_inst->is_dot_string) {
        for (i = 0;i < _inst->num_words_to_generate;i++) {
            _inst->binary_word_vec[i] = (_inst->tokens[1][i]);
        }

        return;
    }

}


void assign_bits_operation(inst_table *_inst_table, size_t index) {
    inst *_inst = _inst_table->inst_vec[index];
    uint16_t combined_word = 0;
    size_t operand_count = 0;
    addressing_method _addressing_method = NO_ADDRESSING_METHOD;
    int i;

    _inst->bin_ARE = 0b100;


    switch (_inst->src_addressing_method) {
    case IMMEDIATE:
        _inst->bin_src_method = 0b0001;
        break;
    case DIRECT:
        _inst->bin_src_method = 0b0010;
        break;
    case INDIRECT_REGISTER:
        _inst->bin_src_method = 0b0100;
        break;
    case DIRECT_REGISTER:
        _inst->bin_src_method = 0b1000;
        break;

    }
    switch (_inst->dest_addressing_method) {
    case IMMEDIATE:
        _inst->bin_dest_method = 0b0001;
        break;
    case DIRECT:
        _inst->bin_dest_method = 0b0010;
        break;
    case INDIRECT_REGISTER:
        _inst->bin_dest_method = 0b0100;
        break;
    case DIRECT_REGISTER:
        _inst->bin_dest_method = 0b1000;
        break;

    }



    combined_word = (_inst->bin_ARE << ARE_SHIFT) | /* ARE bits (bits 0, 1, 2)*/
        (_inst->bin_dest_method << DEST_SHIFT) | /* dest method (bits 3, 4, 5, 6*/
        (_inst->bin_src_method << SRC_SHIFT) | /* src method (bits 7, 8, 9, 10)*/
        (_inst->bin_opcode << OPCODE_SHIFT); /* opcode (bits 11, 12, 13, 14)*/


    /*Assign the first word*/
    _inst->binary_word_vec[0] = combined_word;
    combined_word = 0;

    /*Get the number of operands the operation operates on*/
    operand_count = _inst->num_tokens - 1;

    /*
    * rts
    * stop
    */
    if (operand_count == 0) {
        return; /*No addressing methods*/
    }

    /*
    * clr
    * not
    * inc
    * dec
    * jmp
    * bne
    * red
    * prn
    * jsr
    */
    if (operand_count == 1) {
        _addressing_method = _inst->dest_addressing_method;

        switch (_addressing_method) {
        case IMMEDIATE:
            _inst->binary_word_vec[1] = to_twos_complement(_inst->immediate_val_dest);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] << DEST_SHIFT);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            return;
        case DIRECT:
            _inst->bin_ARE = 0b010;
            _inst->binary_word_vec[1] = to_twos_complement(_inst->direct_label_address_dest);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] << DEST_SHIFT);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            return;
        case INDIRECT_REGISTER:
            _inst->binary_word_vec[1] = (_inst->indirect_reg_num_dest << DEST_SHIFT);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            return;
        case DIRECT_REGISTER:
            _inst->binary_word_vec[1] = (_inst->direct_reg_num_dest << DEST_SHIFT);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            return;
        }
    }


    /*
     * mov
     * cmp
     * add
     * sub
     * lea
     */
    if (operand_count == 2) {
        _addressing_method = _inst->src_addressing_method;
        switch (_addressing_method) {
        case IMMEDIATE:
            _inst->binary_word_vec[1] = to_twos_complement(_inst->immediate_val_src);
            _inst->binary_word_vec[1] = _inst->binary_word_vec[1] << DEST_SHIFT;
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            break;
        case DIRECT:
            _inst->binary_word_vec[1] = to_twos_complement(_inst->direct_label_address_src);
            _inst->binary_word_vec[1] = _inst->binary_word_vec[1] << DEST_SHIFT;
            _inst->bin_ARE = 0b010;
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            break;
        case INDIRECT_REGISTER:
            _inst->binary_word_vec[1] = (_inst->indirect_reg_num_src << SRC_SHIFT);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            break;

        case DIRECT_REGISTER:
            _inst->binary_word_vec[1] = (_inst->direct_reg_num_src << SRC_SHIFT);
            _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
            break;
        }

        _addressing_method = _inst->dest_addressing_method;
        switch (_addressing_method) {
        case IMMEDIATE:
            _inst->binary_word_vec[2] = to_twos_complement(_inst->immediate_val_dest);
            _inst->binary_word_vec[2] = _inst->binary_word_vec[2] << DEST_SHIFT;
            _inst->binary_word_vec[2] = (_inst->binary_word_vec[2] | _inst->bin_ARE);
            return;
        case DIRECT:
            _inst->binary_word_vec[2] = to_twos_complement(_inst->direct_label_address_dest);
            _inst->binary_word_vec[2] = _inst->binary_word_vec[2] << DEST_SHIFT;
            _inst->bin_ARE = 0b010;
            _inst->binary_word_vec[2] = (_inst->binary_word_vec[2] | _inst->bin_ARE);
            return;
        case INDIRECT_REGISTER:
            if (_inst->num_words_to_generate == 2) {
                _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] |
                    _inst->indirect_reg_num_dest << DEST_SHIFT);
                _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
                break;
            }

            _inst->binary_word_vec[2] = _inst->indirect_reg_num_dest << DEST_SHIFT;
            _inst->binary_word_vec[2] = (_inst->binary_word_vec[2] | _inst->bin_ARE);
            return;
        case DIRECT_REGISTER:
            if (_inst->num_words_to_generate == 2) {
                _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] |
                    _inst->direct_reg_num_dest << DEST_SHIFT);
                _inst->binary_word_vec[1] = (_inst->binary_word_vec[1] | _inst->bin_ARE);
                break;
            }

            _inst->binary_word_vec[2] = _inst->direct_reg_num_dest << DEST_SHIFT;
            _inst->binary_word_vec[2] = (_inst->binary_word_vec[2] | _inst->bin_ARE);

            return;
        }

    }



}

status generate_binary_words(inst_table *_inst_table, size_t index) {
    size_t i;
    inst *current_inst = _inst_table->inst_vec[index];

    current_inst->binary_word_vec = (uint16_t *)calloc(current_inst->num_words_to_generate, sizeof(uint16_t));
    if (current_inst->binary_word_vec == NULL) {
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return STATUS_OK;
}


status parse(inst_table *_inst_table, label_table *_label_table, keyword *keyword_table) {
    size_t i;
    size_t bin_word_index;
    size_t inst_index;
    inst *tmp_inst = NULL;
    int address = 100;

    for (i = 0;i < _inst_table->num_instructions;i++) {
        if (generate_binary_words(_inst_table, i) != STATUS_OK) {
            destroy_instruction_table(&_inst_table);
            destroy_label_table(&_label_table);
            destroy_keyword_table(&keyword_table);
            return STATUS_ERROR;
        }

        tmp_inst = _inst_table->inst_vec[i];
        if (!(tmp_inst->is_dot_data || tmp_inst->is_dot_string || tmp_inst->is_entry || tmp_inst->is_extern)) {
            assign_bits_operation(_inst_table, i);
        }

        else assign_bits_directive(_inst_table, i);

    }

    for (inst_index = 0;inst_index < _inst_table->num_instructions;inst_index++) {
        for (i = 0;i < _inst_table->inst_vec[inst_index]->num_tokens;i++) {
            printf("%s ", _inst_table->inst_vec[inst_index]->tokens[i]);
        }
        printf("\n");
        for (bin_word_index = 0;bin_word_index < _inst_table->inst_vec[inst_index]->num_words_to_generate;bin_word_index++) {
            printf("#%d\t ", address);
            print_binary(_inst_table->inst_vec[inst_index]->binary_word_vec[bin_word_index]);
            address++;
        }

        printf("\n");


    }

    destroy_keyword_table(&keyword_table);
    destroy_label_table(&_label_table);
    destroy_instruction_table(&_inst_table);

    return STATUS_OK;

}
