#ifndef DECODE_TO_INT_ARRAY_H
#define DECODE_TO_INT_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_table.h"

int* decode_pre_decoded(string* pre_decoded , int pre_decoded_size, keyword* keyword_table , int key , label* label_table);
int decode_pre_decoded_command(string* pre_decoded, int pre_decoded_size, keyword* keyword_table, int key, label* label_table);
int decode_pre_decoded_command_opcode(keyword* keyword_table, int key);
int addressing_methods_are_legal(int* binary_command , keyword* keyword_table , int key);
int addressing_methods_are_legal_source(int* binary_command , keyword* keyword_table , int key);
int addressing_methods_are_legal_destination(int* binary_command , keyword* keyword_table , int key);
int* decode_pre_decoded_data(int which_data , int* post_decoded , string* pre_decoded , int pre_decoded_size, label* label_table);
int* decode_pre_decoded_data_known_arguments_amount(string* pre_decoded , int pre_decoded_size, int* post_decoded, label* label_table);
int* decode_pre_decoded_data_unknown_arguments_amount(string* pre_decoded);
int decode_2_registers_command( char* source , char* dest);
int map_argument_data(char* str , int addressing_method, label* label_table, int source_or_dest);
int which_register(char* reg);
int calc_methods_2_3(int number, int pos);

#endif //DECODE_TO_INT_ARRAY_H