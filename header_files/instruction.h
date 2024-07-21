#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "common.h"
#include "symbols.h"
#include "util.h"
#include "parser.h"
#include "asm_error.h"

#define INITIAL_NUM_TOKENS 15


/* Create functions */
status create_empty_token(inst *_inst);
status create_instruction(inst **_new_inst);
status create_instruction_table(inst_table **new_inst_table);

/* Init functions */
void init_tokens(inst *_inst);
status init_instructions(inst *_inst);
status init_instruction_table(inst_table *_inst_table);


/* Insert functions */
status insert_token_to_inst(inst *_inst, char *token);
status insert_inst_to_table(inst_table *_inst_table, inst *_inst);

/* Get functions */
char **get_tokens(inst *_inst);
int get_num_tokens(inst *inst);
inst **get_instructions(inst_table *_inst_table);
int get_num_instructions(inst_table *_inst_table);

/* Destroy functions */
void destroy_instruction_table(inst_table *_inst_table);
void destroy_instruction(inst *_inst);


/* Utility */
void print_instruction(inst *_inst);
void print_instruction_table(inst_table *_inst_table);
#endif