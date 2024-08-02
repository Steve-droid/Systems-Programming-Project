#ifndef PRE_ASSEMBLER_H_
#define PRE_ASSEMBLER_H_
#define _POSIX_C_SOURCE 200809L

#include "common.h"
#include "macro.h"
#include "file_util.h"

#define MAX_MACRO_NAME_LENGTH 73
#define MAX_MACRO_AMOUNT 100
#define DEFINE_SEQUENCE_LEN 5

macro_table *fill_macro_table(char *am_filename, char *as_filename, keyword *keyword_table);
#endif