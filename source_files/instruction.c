#include "instruction.h"
#define MAX_TOKEN_SIZE 81
#define INITIAL_CAPACITY 10
#define UNSET -1

static void table_memory_error(inst_table *_inst_table, inst *_inst) {
    destroy_instruction(&_inst);
    destroy_instruction_table(&_inst_table);
    printf("Error: Memory allocation error\n");
}

/* Create functions */
status create_instruction(inst **new_instruction) {
    inst *_inst = NULL;
    size_t i;

    if (new_instruction == NULL) {
        return STATUS_ERROR;
    }

    (*new_instruction) = NULL;
    /* Allocate memory for the new instruction */
    _inst = (inst *)malloc(sizeof(inst));
    if (_inst == NULL) {
        printf("Error while allocating memory for new instruction. Exiting...\n");

        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Allocate memory for the tokens of the new instruction */
    _inst->tokens = (char **)calloc(INITIAL_NUM_TOKENS, sizeof(char *));
    if (_inst->tokens == NULL) {
        free(_inst);
        printf("Error while allocating memory for new instruction\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Set the default values */
    _inst->num_tokens = 0;
    _inst->capacity = INITIAL_NUM_TOKENS;
    _inst->num_words_to_generate = 0;
    _inst->opcode = UNSET;
    _inst->cmd_key = UNSET;
    _inst->address = UNSET;
    _inst->line_number = UNSET;
    _inst->label_key = UNSET;

    /* Directibe parameters */
    _inst->num_dot_data_members = 0;
    _inst->num_dot_string_members = 0;
    _inst->is_dot_data = false;
    _inst->is_dot_string = false;
    _inst->is_entry = false;
    _inst->is_extern = false;
    _inst->is_src_entry = false;
    _inst->is_src_extern = false;
    _inst->is_dest_entry = false;
    _inst->is_dest_extern = false;

    /* Operation parameters */
    _inst->src_addressing_method = NO_ADDRESSING_METHOD;
    _inst->dest_addressing_method = NO_ADDRESSING_METHOD;


    /* For immediate addressing */
    _inst->immediate_val_src = UNDEFINED;
    _inst->immediate_val_dest = UNDEFINED;


    /* For direct addressing */
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        _inst->direct_label_name_src[i] = '\0';
    }
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        _inst->direct_label_name_dest[i] = '\0';
    }
    _inst->direct_label_key_src = UNSET;
    _inst->direct_label_key_dest = UNSET;
    _inst->direct_label_address_src = UNSET;
    _inst->direct_label_address_dest = UNSET;

    /* For indirect register addressing */
    _inst->indirect_reg_num_src = UNSET;
    _inst->indirect_reg_num_dest = UNSET;

    /* For direct register addressing */
    _inst->direct_reg_num_src = UNSET;
    _inst->direct_reg_num_dest = UNSET;


    /* Binary representation parameters */
    _inst->binary_word_vec = NULL;
    _inst->bin_ARE = 0;
    _inst->bin_opcode = 0;
    _inst->bin_src_method = 0;
    _inst->bin_dest_method = 0;
    _inst->bin_address = 0;


    *(new_instruction) = _inst;

    _inst = NULL;
    return STATUS_OK;
}

status create_empty_token(inst *instruction) {
    char *empty_token = NULL;
    size_t i;

    /* Allocate memory for the empty token */
    empty_token = (char *)calloc(MAX_TOKEN_SIZE, sizeof(char));
    if (empty_token == NULL) {
        printf("Error while allocating memory for empty token. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Set the empty token */
    for (i = 0; i < MAX_TOKEN_SIZE; i++) empty_token[i] = '\0';

    /* Insert the empty token to the instruction */
    return insert_token_to_inst(instruction, empty_token);
}

status create_instruction_table(inst_table **new_instruction_table) {
    inst_table *new_table = NULL;

    /* Allocate memory for the new instruction table */
    new_table = (inst_table *)calloc(1, sizeof(inst_table));
    if (new_table == NULL) {
        printf("Error while allocating memory for new instruction table. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Allocate memory for the instructions of the new instruction table */
    new_table->inst_vec = (inst **)calloc(INITIAL_CAPACITY, sizeof(inst *));
    if (new_table->inst_vec == NULL) {
        table_memory_error(new_table, NULL);
    }

    /* Set the default values */
    new_table->num_instructions = 0;
    new_table->capacity = INITIAL_CAPACITY;


    *new_instruction_table = new_table;

    return STATUS_OK;
}

/* Insert functions */
status insert_token_to_inst(inst *instruction, char *token) {
    if (token == NULL) {
        printf(" Trying to insert NULL token to instruction. Exiting...\n");
        return STATUS_ERROR;
    }

    /* Reallocate memory for the tokens if needed */
    if (instruction->num_tokens == instruction->capacity) {
        instruction->capacity += 1;
        instruction->tokens = (char **)realloc(instruction->tokens, sizeof(char *) * instruction->capacity);
        if (instruction->tokens == NULL) {
            printf("Error while reallocating memory for tokens. Exiting...\n");
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    /* Insert the token to the instruction */
    instruction->tokens[instruction->num_tokens] = token;
    instruction->num_tokens++;


    return STATUS_OK;
}

status insert_inst_to_table(inst_table *_inst_table, inst *instruction) {
    if (_inst_table == NULL) {
        printf(" Trying to insert instruction to NULL table. Exiting...\n");
        destroy_instruction(&instruction);
        return ERROR_NULL_TABLE;
    }

    if (instruction == NULL) {
        printf(" Trying to insert NULL instruction to table. Exiting...\n");
        return ERROR_NULL_INSTRUCTION;
    }

    /* Reallocate memory for the instructions if needed */
    if (_inst_table->num_instructions == _inst_table->capacity) {
        _inst_table->capacity += 1;
        _inst_table->inst_vec = (inst **)realloc(_inst_table->inst_vec, sizeof(inst *) * _inst_table->capacity);
        if (_inst_table->inst_vec == NULL) {
            printf("Error while reallocating memory for instructions. Exiting...\n");
            destroy_instruction(&instruction);
            destroy_instruction_table(&_inst_table);
            return STATUS_ERROR;
        }
    }

    /* Insert the instruction to the table */
    _inst_table->inst_vec[_inst_table->num_instructions] = instruction;

    /* Increment the number of instructions */
    _inst_table->num_instructions++;

    return STATUS_OK;
}

void destroy_instruction(inst **_inst) {
    size_t i;
    if (_inst == NULL) {
        return;
    }
    if (*(_inst) != NULL) {
        free((*_inst)->binary_word_vec);
        (*_inst)->binary_word_vec = NULL;


        for (i = 0; i < (*_inst)->capacity; i++) {
            free((*_inst)->tokens[i]);
            (*_inst)->tokens[i] = NULL;
        }
        free((*_inst)->tokens);
        (*_inst)->tokens = NULL;

    }
    free(*_inst);
    (*_inst) = NULL;
}

void destroy_instruction_table(inst_table **_inst_table) {
    int i;

    if (_inst_table == NULL) {
        return;
    }
    if ((*_inst_table) && (*_inst_table)->inst_vec != NULL) {
        /* Free the instructions */
        for (i = 0; i < (*_inst_table)->capacity; i++) {
            destroy_instruction(&((*_inst_table)->inst_vec[i]));
        }
        /* Free instruction vector */
        free((*_inst_table)->inst_vec);
        (*_inst_table)->inst_vec = NULL;

    }



    free((*_inst_table));
    (*_inst_table) = NULL;
}

void print_instruction(inst *_inst, label_table *_label_table) {
    size_t i;
    size_t size = 0;
    char addressing_method_src[MAX_LINE_LENGTH] = { 0 };
    char addressing_method_dest[MAX_LINE_LENGTH] = { 0 };
    label *tmp_label = NULL;
    if (_inst == NULL) {
        printf(" Trying to print NULL instruction\n");
        return;
    }

    if (_inst->is_entry || _inst->is_extern) return;

    if (_inst->tokens == NULL) {
        printf(" Trying to print instruction with NULL tokens\n");
        return;
    }

    if (_inst->src_addressing_method == DIRECT) {
        strcpy(addressing_method_src, "DIRECT");
    }
    else if (_inst->src_addressing_method == IMMEDIATE) {
        strcpy(addressing_method_src, "IMMEDIATE");
    }
    else if (_inst->src_addressing_method == INDIRECT_REGISTER) {
        strcpy(addressing_method_src, "INDIRECT_REGISTER");
    }
    else if (_inst->src_addressing_method == DIRECT_REGISTER) {
        strcpy(addressing_method_src, "DIRECT_REGISTER");
    }
    else if (_inst->src_addressing_method == NO_ADDRESSING_METHOD) {
        strcpy(addressing_method_src, "NO_ADDRESSING_METHOD");
    }

    if (_inst->dest_addressing_method == DIRECT) {
        strcpy(addressing_method_dest, "DIRECT");
    }
    else if (_inst->dest_addressing_method == IMMEDIATE) {
        strcpy(addressing_method_dest, "IMMEDIATE");
    }
    else if (_inst->dest_addressing_method == INDIRECT_REGISTER) {
        strcpy(addressing_method_dest, "INDIRECT_REGISTER");
    }
    else if (_inst->dest_addressing_method == DIRECT_REGISTER) {
        strcpy(addressing_method_dest, "DIRECT_REGISTER");
    }
    else if (_inst->dest_addressing_method == NO_ADDRESSING_METHOD) {
        strcpy(addressing_method_dest, "NO_ADDRESSING_METHOD");
    }


    size = _inst->num_tokens;
    printf("\n\t***\t");
    for (i = 0; i < size; i++) {
        printf("%s ", _inst->tokens[i]);
    }
    printf("\t***");
    printf("\n\nGeneral Information:\n");
    printf("\t\tLine number: %ld\n", _inst->line_number);
    printf("\t\tNumber of tokens: %ld\n", _inst->num_tokens);
    printf("\t\tCommand key: %d\n", _inst->cmd_key);
    if (_inst->opcode != UNSET) {
        printf("\t\tOpcode: ");
        print_bits(_inst->bin_opcode, 4);
        printf("\n");
    }

    printf("\t\tNumber of binary words to generate: %lu\n", _inst->num_words_to_generate);

    if (_inst->address == UNSET)
        printf("\t\tAddress: Unset\n");
    else
        printf("\t\tAddress: %d\n", _inst->address);

    if (_inst->label_key != UNSET) {
        tmp_label = get_label_by_key(_label_table, _inst->label_key);
        printf("\t\tInstruction is defined by the following label:\n");
        printf("\t\tLabel name: '%s', Label key: %d\n", tmp_label->name, _inst->label_key);
    }

    if (_inst->src_addressing_method != NO_ADDRESSING_METHOD) {
        printf("\nSource Operand Information:\n");
        printf("\t\tSource operand addressing method --> %s\n", addressing_method_src);
    }

    if (_inst->immediate_val_src != UNDEFINED) {
        printf("\t\tImmediate Addressing --> value of source operand --> %d\n", _inst->immediate_val_src);
    }

    if (_inst->direct_label_key_src != UNSET) {
        printf("\t\tDirect Addressing --> label key for source operand: %d\n", _inst->direct_label_key_src);
        printf("\t\tDirect Addressing --> label name for source operand: %s\n", _inst->direct_label_name_src);
    }

    if (_inst->indirect_reg_num_src != UNSET) {
        printf("\t\tIndirect Register Addressing --> source register number --> %d\n", _inst->indirect_reg_num_src);
    }

    if (_inst->direct_reg_num_src != UNSET) {
        printf("\t\tDirect Regsiter Addressing --> source register number --> %d\n", _inst->direct_reg_num_src);
    }

    if (_inst->dest_addressing_method != NO_ADDRESSING_METHOD) {
        printf("\nDestination Operand Information:\n");
        printf("\t\tDestination operand addressing method --> %s\n", addressing_method_dest);
    }

    if (_inst->immediate_val_dest != UNDEFINED) {
        printf("\t\tImmediate Addressing --> value of destination operand --> %d\n", _inst->immediate_val_dest);
    }

    if (_inst->direct_label_key_dest != UNSET) {
        printf("\t\tDirect Addressing --> label key for destination operand --> %d\n", _inst->direct_label_key_dest);
        printf("\t\tDirect Addressing --> label name for destination operand --> %s\n", _inst->direct_label_name_dest);
    }

    if (_inst->indirect_reg_num_dest != UNSET) {
        printf("\t\tIndirect Register Addressing --> destination register number --> %d\n",
            _inst->indirect_reg_num_dest);
    }

    if (_inst->direct_reg_num_dest != UNSET) {
        printf("\t\tDirect Regsiter Addressing --> destination register number --> %d\n", _inst->direct_reg_num_dest);
    }
    printf("---------------------------------------------------------\n");
}

void print_instruction_table(inst_table *_inst_table, label_table *_label_table) {
    size_t i;
    if (_inst_table == NULL) {
        printf("Trying to print NULL instruction table\n");
        return;
    }
    printf("\n######################################################\n");
    printf("Printing instruction table:");
    printf("\n######################################################\n");
    printf("Number of instructions: %ld\n", _inst_table->num_instructions);
    printf("Capacity: %ld\n", _inst_table->capacity);
    printf("IC: %ld\n", _inst_table->IC);
    printf("DC: %ld\n", _inst_table->DC);
    printf("\n---------------------------------------------------------\n");
    printf("Printing instructions:");
    printf("\n---------------------------------------------------------\n");


    for (i = 0; i < _inst_table->num_instructions; i++) {
        print_instruction(_inst_table->inst_vec[i], _label_table);
    }
    printf("\n######################################################\n");
    printf("End of instruction table");
    printf("\n######################################################\n\n\n");
}

size_t DC(int prompt, size_t amount)
{
    static size_t _DC = 0;
    if (prompt == RESET) {
        _DC = 0;
        return _DC;
    }

    if (prompt == GET)
        return _DC;

    if (prompt == INCREMENT) {
        _DC += amount;
    }

    return _DC;
}

size_t IC(int prompt, size_t amount)
{
    static size_t _IC = 100;

    if (prompt == RESET) {
        _IC = 100;
        return _IC;
    }
    if (prompt == GET)
        return _IC;
    if (prompt == INCREMENT) {
        _IC += amount;
    }

    return _IC;
}
