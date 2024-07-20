#ifndef BINARY_H
#define BINARY_H
#include "common.h"



/* Init functions */
void init_word(bin_word *word);
void init_inst(bin_inst *instruction);
void init_table(bin_table *table);

/* Create functions */
status create_bin_word(bin_inst *_instr);
status create_bin_inst(bin_table *_bin_table);
status create_bin_table(bin_table **new_bin_table);

/* Insert functions */
status insert_bin_word_to_inst(bin_inst *instruction, bin_word *word);
status insert_bin_inst_to_table(bin_table *table, bin_inst *instruction);

/* Get functions */
bin_word **get_words(bin_inst *instruction);
size_t get_num_words(bin_inst *instruction);
bin_inst **get_bin_inst(bin_table *table);
size_t get_num_bin_inst(bin_table *table);

/* Destroy functions */
void destroy_bin_word(bin_word *word);
void destroy_bin_inst(bin_inst *instruction);
void destroy_bin_table(bin_table *table);



#endif