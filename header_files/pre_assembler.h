#ifndef PRE_ASSEMBLER_H_
#define PRE_ASSEMBLER_H_
#define _POSIX_C_SOURCE 200809L

#include "common.h"
#include "macro.h"
#include "util.h"

#define MAX_MACRO_NAME_LENGTH 73
#define MAX_MACRO_AMOUNT 100
#define DEFINE_SEQUENCE_LEN 5



status pre_assemble(char *as_filename, char *am_filename, macro_table *m_table);
macro_table *fill_macro_table(int argc, char *argv[], char ***am_filenames);


#endif