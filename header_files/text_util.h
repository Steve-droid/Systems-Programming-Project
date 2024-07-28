#ifndef TEXT_TOOLS_H
#define TEXT_TOOLS_H

#include "common.h"
#include "text_util.h"
#include "symbols.h"
#include "lexer.h"
#include "instruction.h"



/*Syntax State utility*/
syntax_state *create_syntax_state();

void initialize_command(syntax_state *data);

void update_command(syntax_state *state, keyword *keyword_table, int command_key);

bool continue_reading(char *instruction_buffer, syntax_state *state);

void reset_syntax_state(syntax_state *state);

void destroy_syntax_state(syntax_state **state);

/*Operations on characters and pointers*/
char *trim_whitespace(char *str);

void str_cpy_until_char(char *destination, const char *source, char x);

void initialize_char_array(char *char_array);

void skip_label_name(syntax_state *state, label_table *_label_table);

char *skip_ent_or_ext(char *_buffer);

int *convert_to_int_array(char *str);
void int_to_binary_array(int num, bin_word *binary_word, int start, int finish);
int binary_array_to_int(int *array);
bool is_empty_line(char *str);
void print_2D_array(int **arr);
void print_array_in_binary(int *arr);
int *convert_twodim_array_to_onedim(int **two_dim_array);
void print_binary(uint16_t num, FILE *file_ptr);
void print_binary_2(int num);
void print_octal(uint16_t number, FILE *file_ptr);



#endif