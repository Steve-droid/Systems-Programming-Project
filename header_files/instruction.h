#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "ds.h"
#include "symbols.h"
#include "text_util.h"
#include "asm_error.h"

#define INITIAL_NUM_TOKENS 3


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

/* Destroy functions */
void destroy_instruction_table(inst_table **_inst_table);
void destroy_instruction(inst **_inst);


/* Utility */
void print_instruction(inst *_inst, label_table *_label_table);
void print_instruction_table(inst_table *_inst_table, label_table *_label_table);
#endif
