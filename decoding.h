#ifndef DECODING_H
#define DECODING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_table.h"

#define UNKNOWN_NUMBER_OF_ARGUMENTS -2
#define NO_NEED_TO_DECODE -10
#define OUTPUT_COMMAND_LEN 15
#define OPCODE_LEN 4
#define ADDRESSING_METHOD_LEN 4
#define A 12
#define R 13
#define E 14 
#define SOURCE 2
#define DEST 1
#define FIRST_ADDRESS 100

/* Declerations */
int* decoding(char* am_filename, label* label_table, keyword* keyword_table);
void fill_label_table_addresses(int** decoded_table , label* label_table);
void decode_label_addersses(int* decoded , label* label_table);

#endif /* DECODING_H */