#include "instruction.h"
#define MAX_TOKEN_SIZE 25

/* Error handling functions */
static void generic_memory_error(void *allocated_data) {
    free(allocated_data);
    printf("Error: Memory allocation error\n");
}

static void table_memory_error(inst_table *_inst_table, inst *_inst) {
    destroy_instruction(_inst);
    destroy_instruction_table(_inst_table);
    printf("Error: Memory allocation error\n");
}


/* Create functions */
status create_instruction(inst **new_instruction) {
    inst *new_inst = NULL;

    /* Allocate memory for the new instruction */
    new_inst = (inst *)malloc(sizeof(inst));
    if (new_inst == NULL) {
        printf("Error while allocating memory for new instruction. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Allocate memory for the tokens of the new instruction */
    new_inst->tokens = (char **)malloc(sizeof(char *) * INITIAL_NUM_TOKENS);
    if (new_inst->tokens == NULL) {
        printf("Error while allocating memory for new instruction. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Set the default values */
    new_inst->num_tokens = 0;
    new_inst->capacity = INITIAL_NUM_TOKENS;
    new_inst->cmd_key = UNDEFINED;
    *new_instruction = new_inst;
    return STATUS_OK;
}

status create_empty_token(inst *instruction) {
    char *empty_token = NULL;
    size_t i;

    /* Allocate memory for the empty token */
    empty_token = (char *)malloc(MAX_TOKEN_SIZE);
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
    new_table = (inst_table *)malloc(sizeof(inst_table));
    if (new_table == NULL) {
        printf("Error while allocating memory for new instruction table. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* Allocate memory for the instructions of the new instruction table */
    new_table->inst_vec = (inst **)malloc(sizeof(inst *) * INITIAL_NUM_TOKENS);
    if (new_table->inst_vec == NULL) {
        table_memory_error(new_table, NULL);
    }

    /* Set the default values */
    new_table->num_instructions = 0;
    new_table->capacity = 4;
    *new_instruction_table = new_table;

    return STATUS_OK;
}

status init_instruction(inst *_inst) {
    _inst->num_tokens = 0;
    _inst->capacity = INITIAL_NUM_TOKENS;
    _inst->cmd_key = UNDEFINED;
    _inst->line_number = UNDEFINED;
    _inst->is_dot_data = false;
    _inst->is_dot_string = false;
    _inst->is_entry = false;
    _inst->is_extern = false;
    _inst->tokens = (char **)malloc(sizeof(char *) * _inst->capacity);
    if (_inst->tokens == NULL) {
        printf("Error while allocating memory for tokens. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return STATUS_OK;
}

status init_instruction_table(inst_table *_inst_table) {
    size_t i;
    _inst_table->num_instructions = 0;
    _inst_table->capacity = 4;
    _inst_table->inst_vec = (inst **)malloc(sizeof(inst *) * _inst_table->capacity);
    if (_inst_table->inst_vec == NULL) {
        printf("Error while allocating memory for instruction table. Exiting...\n");
        return STATUS_ERROR_MEMORY_ALLOCATION;
    }

    for (i = 0; i < _inst_table->capacity; i++) _inst_table->inst_vec[i] = NULL;

    return STATUS_OK;
}

/* Insert functions */
status insert_token_to_inst(inst *instruction, char *token) {

    if (token == NULL) {
        printf("Error: Trying to insert NULL token to instruction. Exiting...\n");
        return STATUS_ERROR;
    }

    /* Reallocate memory for the tokens if needed */
    if (instruction->num_tokens == instruction->capacity) {
        instruction->tokens = (char **)realloc(instruction->tokens, sizeof(char *) * instruction->capacity * 2);
        if (instruction->tokens == NULL) {
            printf("Error while reallocating memory for tokens. Exiting...\n");
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
        instruction->capacity *= 2;
    }

    /* Insert the token to the instruction */
    instruction->tokens[instruction->num_tokens] = token;
    instruction->num_tokens++;

    /* Set the pointer to the next token that will be inserted to NULL */
    instruction->tokens[instruction->num_tokens] = NULL;

    return STATUS_OK;
}

status insert_inst_to_table(inst_table *_inst_table, inst *instruction) {

    if (_inst_table == NULL) {
        printf("Error: Trying to insert instruction to NULL table. Exiting...\n");
        return ERROR_NULL_TABLE;
    }

    if (instruction == NULL) {
        printf("Error: Trying to insert NULL instruction to table. Exiting...\n");
        return ERROR_NULL_INSTRUCTION;
    }

    /* Reallocate memory for the instructions if needed */
    if (_inst_table->num_instructions == _inst_table->capacity) {
        _inst_table->inst_vec = (inst **)realloc(_inst_table->inst_vec, sizeof(inst *) * _inst_table->capacity * 2);
        if (_inst_table->inst_vec == NULL) {
            printf("Error while reallocating memory for instructions. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        _inst_table->capacity *= 2;
    }

    /* Insert the instruction to the table */
    _inst_table->inst_vec[_inst_table->num_instructions] = instruction;

    /* Increment the number of instructions */
    _inst_table->num_instructions++;

    /* Set the pointer to the next instruction that will be inserted to NULL */
    _inst_table->inst_vec[_inst_table->num_instructions] = NULL;

    return STATUS_OK;

}


/* Get functions */
char **get_tokens(inst *instruction) {
    if (instruction == NULL) {
        printf("Error: Trying to access tokens from a NULL instruction\n");
        return NULL;
    }
    return instruction->tokens;
}

int get_num_tokens(inst *instruction) {
    if (instruction == NULL) {
        printf("Error: Trying to access number of tokens from a NULL instruction\n");
        return UNDEFINED;
    }

    return instruction->num_tokens;
}

inst **get_instructions(inst_table *_inst_table) {
    if (_inst_table == NULL) {
        printf("Error: Trying to access instructions from a NULL table\n");
        return NULL;
    }

    if (_inst_table->inst_vec == NULL) {
        printf("Error: Trying to access instructions from a NULL instruction vector\n");
        return NULL;
    }

    return _inst_table->inst_vec;
}

inst *get_instruction_at(inst_table *_table, size_t index) {
    if (_table == NULL) return NULL;

    if (index >= _table->num_instructions) {
        printf("Error: Trying to access instruction at index %ld, but the table has only %ld instructions\n", index, _table->num_instructions);
        return NULL;
    }

    return _table->inst_vec[index];
}

int get_num_instructions(inst_table *table) {
    if (table == NULL) {
        printf("Error: Trying to access number of instructions from a NULL table\n");
        return UNDEFINED;
    }

    return table->num_instructions;
}



/* Destroy functions */
void destroy_tokens(inst *instruction) {
    if (instruction->tokens == NULL) return;
    free(instruction->tokens);
}

void destroy_instruction(inst *instruction) {
    if (instruction == NULL) return;
    destroy_tokens(instruction);
    free(instruction);
}

void destroy_instruction_table(inst_table *_inst_table) {
    size_t i;

    if (_inst_table == NULL) return;

    /* Free the instructions */
    for (i = 0; i < _inst_table->num_instructions; i++) {
        destroy_instruction(_inst_table->inst_vec[i]);
    }

    /* Free instruction vector */
    free(_inst_table->inst_vec);
    free(_inst_table);
}































