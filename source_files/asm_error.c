#include "asm_error.h"

void quit_main(int file_amount, filenames **fnames, macro_table **_macro_table, keyword **keyword_table, label_table **_label_table, inst_table **_instruction_table) {
   quit_filename_creation(fnames);
   macro_table_destructor(_macro_table);
   destroy_keyword_table(keyword_table);
   destroy_label_table(_label_table);
   destroy_instruction_table(_instruction_table);
}

void quit_filename_creation(filenames **fnames) {
   int i;
   filenames *names = NULL;
   if (fnames == NULL || (*fnames == NULL) || (*fnames)->amount == 0)
      return; /* Memory already freed */

   names = (*fnames);
   if (names->am) {
      for (i = 0;i < (*fnames)->amount;i++) {
         free(names->am[i]);
      }

      free(names->am);
      names->am = NULL;
   }

   if (names->as) {
      for (i = 0;i < (*fnames)->amount;i++) {
         free(names->as[i]);
      }

      free(names->as);
      names->as = NULL;
   }

   if (names->generic) {
      for (i = 0;i < (*fnames)->amount;i++) {
         free(names->generic[i]);
      }

      free(names->generic);
      names->generic = NULL;
   }


   free(names);
   (*fnames) = NULL;

}
