#ifndef LABEL_TABLE_H
#define LABEL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_table.h"

#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */
#define FIRST_KEY 40001 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */

/* Declarations */
label* create_labels_table();
void fill_label_data_exclude_address(label* any_label, char *line, int line_counter);
label* fill_label_table(char* am_filename , macro_table* macroTable , keyword* keywords_table);
void fill_table_size(label* label_table, int labels_counter);
void increase_labels_table(label** table, size_t new_size);
int label_name_is_in_table(label* label_table, char* label_name, int table_size);
int label_name_is_keyword(char* label_name , keyword* keywords_table);
int label_name_is_macro(char* label_name, macro_table* table);
int label_name_is_valid(label* label_table, char* label_name, int table_size , keyword* keywords_table , macro_table* macroTable);
void str_cpy_until_char(char* destination , const char* source , char x);
int string_to_label(char* str , label* label_table);



#endif //LABEL_TABLE_H