#include "parser.h"
#define ARE_SHIFT 0
#define DEST_SHIFT 3
#define SRC_SHIFT 7
#define OPCODE_SHIFT 11

uint16_t to_twos_complement(int16_t number) {
    return (uint16_t)number;
}

static void assign_bits_directive(inst_table *_inst_table, size_t index) {

}

static void handle_immediate(inst_table *_inst_table, size_t index) {

}

static void handle_direct(inst_table *_inst_table, size_t index) {

}

static void handle_indirect_reg(inst_table *_inst_table, size_t index) {

}

static void handle_direct_reg(inst_table *_inst_table, size_t index) {

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



    uint16_t combined_word = (_inst->bin_ARE << ARE_SHIFT) | /* ARE bits (bits 0, 1, 2)*/
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
            handle_immediate(_inst_table, index);
            return;
        case DIRECT:
            handle_direct(_inst_table, index);
            return;
        case INDIRECT_REGISTER:
            handle_indirect_reg(_inst_table, index);
            return;
        case DIRECT_REGISTER:
            handle_direct_reg(_inst_table, index);
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
    else {
        _addressing_method = _inst->src_addressing_method;
        switch (_addressing_method) {
        case IMMEDIATE:
            handle_immediate(_inst_table, index);
            break;
        case DIRECT:
            handle_direct(_inst_table, index);
            break;
        case INDIRECT_REGISTER:
            handle_indirect_reg(_inst_table, index);
            break;
        case DIRECT_REGISTER:
            handle_direct_reg(_inst_table, index);
            break;
        }

        _addressing_method = _inst->dest_addressing_method;
        switch (_addressing_method) {
        case IMMEDIATE:
            handle_immediate(_inst_table, index);
            return;
        case DIRECT:
            handle_direct(_inst_table, index);
            return;
        case INDIRECT_REGISTER:
            handle_indirect_reg(_inst_table, index);
            return;
        case DIRECT_REGISTER:
            handle_direct_reg(_inst_table, index);
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


status *parse(inst_table *_inst_table, label_table *_label_table, keyword *keyword_table) {
    size_t i;
    inst *tmp_inst = NULL;

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

}
