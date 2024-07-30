#include "asm_error.h"

void terminate(int file_amount, char ***am_filenames, macro_table **_macro_table, keyword **keyword_table, label_table **_label_table, inst_table **_instruction_table) {
   delete_filenames(file_amount, am_filenames);
   macro_table_destructor(_macro_table);
   destroy_keyword_table(keyword_table);
   destroy_label_table(_label_table);
   destroy_instruction_table(_instruction_table);
}