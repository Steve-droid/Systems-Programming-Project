#ifndef DECODE_TO_STRING_ARRAY_H
#define DECODE_TO_STRING_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_table.h"

int** decode_without_label_addresses(label* label_table, keyword* keyword_table);
int deciphering_command(char* line, label* label_table, keyword* keyword_table, int current_line);
int arguments_amount_for_command(int command_name);
int fill_pre_decoded(char* line, string* pre_decoded, int pre_decoded_size, keyword* keyword_table, int command_name_key, label* label_table, int current_line);
int fill_pre_decoded_unknown_arguments_amount(char* line, int command_number, keyword* keyword_table, string* pre_decoded, label* label_table);
int fill_pre_decoded_known_arguments_amount(char* line, string* pre_decoded, int pre_decoded_size, label* label_table);
int check_dotData_numbers(char* str);
int known_argument_data_is_valid(char* str, label* label_table);

#endif //DECODE_TO_STRING_ARRAY_H