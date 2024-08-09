#ifndef ASM_ERROR_H
#define ASM_ERROR_H

#include <stdarg.h>
#include "common.h"
#include "file_util.h"
#include "macro.h"
#include "symbols.h"
#include "instruction.h"

typedef enum {

   /* Memory handling errors */
   m1_general_memerr,
   m2_syntax_state,
   m3_inst_tab_creation,
   m4_inst_creation,
   m5_inst_insert,

   /* File handling errors */
   f1_file_open,

   /* General errors */
   e1_undef_cmd,
   e2_tokgen,
   e3_addr_assign,
   e4_extra_comma,
   e5_missing_args,
   e6_arg_order,
   e7_lea_nondir_src,
   e8_lea_imm_dest


} error_code;

void quit_main(int file_amount, filenames **fnames, macro_table **_macro_table, keyword **keyword_table, label_table **_label_table, inst_table **_instruction_table);
void quit_filename_creation(filenames **fnames);
void quit_lex(syntax_state **state, inst_table **_inst_table, FILE *am_file_ptr);
void my_perror(syntax_state *state, error_code e_code);

#endif
