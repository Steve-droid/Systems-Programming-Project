#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#define INITIAL_NUM_TOKENS 3

#include "data_structs.h"
#include "symbols.h"
#include "text_util.h"
#include "asm_error.h"



/* Create functions */
status create_empty_token(inst *_inst);
status create_instruction(inst **_new_inst);
status create_instruction_table(inst_table **new_inst_table);

/* Insert functions */
status insert_token_to_inst(inst *_inst, char *token);
status insert_inst_to_table(inst_table *_inst_table, inst *_inst);

/* Destroy functions */
void destroy_instruction_table(inst_table **_inst_table);
void destroy_instruction(inst **_inst);

#endif
