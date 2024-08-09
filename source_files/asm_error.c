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

void quit_lex(syntax_state **state, inst_table **_inst_table, FILE *am_file_ptr) {
   destroy_syntax_state(state);
   destroy_instruction_table(_inst_table);
   if (am_file_ptr) fclose(am_file_ptr);
}


void my_perror(syntax_state *state, error_code e_code) {
   if (state == NULL || e_code < 0) return;



   switch (e_code) {

      /* Memory handling errors */
   case m1_general_memerr:
      printf("Failed to allocate memory for assembly process.\n");
      break;
   case m2_syntax_state:
      printf("Failed to allocate memory for syntax state structre.\n");
      break;
   case m3_inst_tab_creation:
      printf("Failed to allocate memory for the instruction table structre.\n");
      break;
   case m5_inst_insert:
      printf("Error in file %s: line %d: '%s' - ", state->as_filename, state->line_number, state->buffer_without_offset);
      printf("Failed to insert instruction to instruction table.\n");


      /* File handling errors */
   case f1_file_open:
      break;
   default:
      break;
   }


   printf("Error in file %s: line %d: '%s' - ", state->as_filename, state->line_number, state->buffer_without_offset);
   switch (e_code) {

      /* General errors */
   case e1_undef_cmd:
      printf("Undefined command name.\n");
      break;

   case e2_tokgen:
      printf("Failed to lex instruction.\n");
      break;


   case e3_addr_assign:
      printf("Failed to assign addresses.\n");
      break;


   case e4_extra_comma:
      printf("Unnecessary comma between the command and the arguments\n");
      break;

   case e5_missing_args:
      printf("Expected %lu arguments for the '%s' command, but recieved only %d\n", state->_inst->num_tokens - 1, state->_inst->tokens[0], state->arg_count);
      break;
   case e6_arg_order:
      printf("Wrong order of arguments.\n");
      break;
   case e7_lea_nondir_src:
      printf("Trying to assign an non-direct addressing method to a source argument of 'lea' instruction\n");
      break;
   case e8_lea_imm_dest:
      printf("Trying to assign an immediate addressing method to a destination argument of 'lea' instruction\n");
      break;



   default:
      break;






   }




}
