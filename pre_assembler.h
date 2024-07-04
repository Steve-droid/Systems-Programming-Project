#ifndef PRE_ASSEMBLER_H_
#define PRE_ASSEMBLER_H_
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macro.h"
#include "vector.h"

#define as_extention ".as"
#define am_extention ".am"
#define MAX_MACRO_NAME_LENGTH 31
#define MAX_MACRO_AMOUNT 100
#define MAX_LINE_LENGTH 81

typedef enum macro_state {
    definition, empty_line, line_other, macro_redefinition_error,
    illegal_definition, illegeal_end_macro, call, illegal_call
}macro_state;

void compute_as_file_name(char *src_file_name, char *as_file_name);

void compute_am_file_name(char *src_file_name, char *am_file_name);

void pre_assemble(char *src_file_name, const char *output_file_path);

FILE *create_am_file(char *am_file_name);

FILE *create_as_file(char *as_file_name);



#endif