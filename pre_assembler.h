#ifndef PRE_ASSEMBLER_H_
#define PRE_ASSEMBLER_H_
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macro.h"
#include "utilities.h"

#define as_extension ".as"
#define am_extension ".am"
#define MAX_MACRO_NAME_LENGTH 73
#define MAX_MACRO_AMOUNT 100
#define MAX_LINE_LENGTH 80
#define BUFFER_SIZE 1024


status pre_assemble(char *as_filename, char *am_filename, macro_table *table);
macro_table *fill_macro_table(int argc, char *argv[], char **am_filename);


#endif