#ifndef ASM_ERROR_H
#define ASM_ERROR_H

#include <stdarg.h>
#include "common.h"
#include "file_util.h"
#include "macro.h"
#include "symbols.h"
#include "instruction.h"

void terminate(int file_amount, char ***am_filenames, macro_table **_macro_table,
   keyword **keyword_table, label_table **_label_table, inst_table **_instruction_table);


#endif