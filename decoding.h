#ifndef MMN14_MAIN_H
#define MMN14_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "make_label_table.h"

typedef struct{
    char data[MAX_LINE_LENGTH];
}string;

#define UNKNOWN_NUMBER_OF_ARGUMENTS 100
#define UNDEFINED -1

/* Declerations */
int* decoding(label* label_table, keyword* keyword_table, int* decoded_array_size);
int arguments_amount_for_command(int command_name);
int deciphering_command(char* line, label* label_table, keyword* keyword_table, int current_line);
void fill_pre_decoded(char* line, string* pre_decoded, int pre_decoded_size, keyword* keyword_table, int command_name_key, label* label_table, int current_line);
char* pointer_after_label(char* line, label* label_table, int current_line);
void fill_pre_decoded_unknown_arguments_amount(char* line, int command_number, keyword* keyword_table, string* pre_decoded);
void fill_pre_decoded_known_arguments_amount();
int command_number_by_key(keyword* keyword_table, int key);
int* decode_pre_decoded(string* pre_decoded , int* post_decoded_size);



#endif //MMN14_MAIN_H