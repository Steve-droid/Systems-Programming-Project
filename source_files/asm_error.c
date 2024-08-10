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
   char *ptr = NULL;
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
      break;
   case m6_tok_gen_mem:
      printf("Failed to allocate memory for tokens while lexing instruction.\n");
      break;



      /* File handling errors */
   case f1_file_open:
      break;
   default:
      break;
   }


   switch (e_code) {
   case n1_prefix_op:
      printf("Note: In file %s: line %d: '%s': ", state->as_filename, state->line_number, state->buffer_without_offset);
      printf("A prefix of the invalid operation name '");
      ptr = state->buffer;
      while (ptr != NULL && !isspace(*ptr) && (*ptr) != '\0') {
         putchar(*ptr);
         ptr++;
      }

      printf("'");


      printf(" is a valid operation name called '%s'. Did you mean to write '%s'? \n",
         state->k_table[state->index].name, state->k_table[state->index].name);

      return;
   default:
      break;
   }


   printf("Syntax Error: In file %s: On line %d: '%s': ", state->as_filename, state->line_number, state->buffer_without_offset);
   switch (e_code) {

      /* General errors */
   case e1_undef_cmd:
      printf("Undefined command name: '");
      ptr = state->buffer;
      while (ptr != NULL && !isspace(*ptr) && (*ptr) != '\0') {
         putchar(*ptr);
         ptr++;
      }

      printf("'\n");

      break;

   case e2_tokgen:
      printf("Failed to lex instruction.\n");
      break;


   case e3_addr_assign:
      printf("Failed to assign addresses.\n");
      break;


   case e4_extra_comma:
      printf("Unnecessary comma between the command '%s' and the first argument\n", state->_inst->tokens[0]);
      break;

   case e5_missing_args:
      printf("Trying to pass less than %lu argument to the '%s' instruction\n", state->_inst->num_tokens - 1, state->_inst->tokens[0]);
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
   case e9_inval_opcode:
      printf("Command opcode not found\n");
      break;
   case e10_series_of_commas:
      printf("A series of commas without an argument between them\n");
      break;
   case e11_missing_comma:
      printf("Missing a comma - ',' between arguments of the instruction\n");
      break;
   case e12_too_much_args:
      printf("Trying to pass more than %lu arguments for the '%s' instruction\n", state->_inst->num_tokens - 1, state->_inst->tokens[0]);
      break;
   case e13_comma_after_last_arg:
      printf("An invalid comma- ',' after the last argument of the '%s' instruction\n", state->buffer_without_offset);
      break;
   case e14_comma_before_data:
      printf("Additional unnecessary comma- ',' before '.data' integer list\n");
      break;
   case e15_inval_data_symbol:
      printf("An invalid symbol '%c' in '.data' integer list\n", state->buffer[state->index]);
      break;
   case e16_no_comma_betw_ints_data:
      printf("Missing a comma - ',' between integers in '.data' definition\n");
      break;
   case e17_extre_comma_data_mid:
      printf("A series of commas without an integer between them\n");
      break;
   case e18_extra_comma_data_end:
      printf("An unnecessary comma in the end of '.data' integer list\n");
      break;
   case e19_pm_wrong_position:
      printf("Invalid position of '+' or '-' sign when defining a '.data' integer list\n");
      break;
   case e20_pm_no_int:
      printf("No matching integer for a '+' or '-' sign when defining a '.data' integer list\n");
      break;
   case e21_str_no_qm:
      printf("Trying to define a string without any quotation marks - '\"'\n");
      break;
   case e22_str_no_qm_start:
      printf("Trying to define a string without opening quotation marks- '\"'\n");
      break;
   case e23_str_no_qm_end:
      printf("Trying to define a string without closing quotation marks- '\"'\n");
      break;
   case e24_inval_label_name:
      printf("Invalid character(non english letter or digit) detected in label name\n");
      break;

   case e25_ent_ex_args:
      printf("Additional arguments in an 'entry'/'extern' instruction\n");
      break;

   case e26_third_assignment:
      printf("Trying to assign addressing methods to arguments for a 3rd time.\n");
      break;

   case e27_inval_addr_mehod:
      printf("Trying to pass an invalid argument '%s' that does not match any addressing method.\n", state->tmp_arg);
      break;

   case e28_inval_method_mov_add_sub:
      printf("Trying to assign an immediate addressing method to a destination argument of a 'mov'/'add'/'sub' instruction\n");
      break;

   case e39_cmp_extra_args:
      printf("Trying to pass more than one argument to a 'cmp' instruction.\n");
      break;

   case e29_inval_method_imm_lea:
      printf("Trying to assign an immediate addressing method to destination argument of a 'lea' instruction\n");
      break;

   case e30_ext_arg_clr_not_inc_dec_red:
      printf("Trying to pass more than one argument as input to a 'clr'/'not'/'inc'/'dec'/'red' instruction\n");
      break;

   case e31_mult_assign_clr_not_inc_dec_red:
      printf("Multiple assignment attempts for a destination argument addressing method of a 'clr'/'not'/'inc'/'dec'/'red' instruction\n");
      break;
   case e32_imm_clr_not_inc_dec_red:
      printf("Trying to assign an immediate addressing method to a destination argument of 'clr'/'not'/'inc'/'dec'/'red'  instruction\n");
      break;

   case e33_tomany_jmp_bne_jsr:
      printf("Trying to pass more than one argument as input to a 'jmp'/'bne'/'jsr' instruction\n");
      break;

   case e34_mul_assign_jmp_bne_jsr:
      printf("Multiple assignment attempts for a destination argument addressing method of a 'jmp'/'bne'/'jsr' instruction\n");
      break;

   case e35_imm_jmp_bne_jsr:
      printf("Trying to assign an immediate addressing method to a destination argument of 'jmp'/'bne'/'jsr'  instruction\n");
      break;

   case e36_dir_jmp_bne_jsr:
      printf("Trying to assign a direct register addressing method to a destination argument of 'jmp'/'bne'/'jsr'  instruction\n");
      break;

   case e37_toomany_prn:
      printf("Trying to pass more than one argument as input to a 'prn' instruction\n");
      break;
   case e38_addr_mult_assign_prn:
      printf("Multiple assignment attempts for a destination argument addressing method of a 'prn' instruction\n");
      break;

   case e40_toomany_rts_stop:
      printf("Trying to pass an argument as input to a 'rts'/'stop' instruction\n");
      break;
   case e41_lbl_arg:
      printf("Trying to use an invalid label name as an argument\n");
      break;

   case e42_imm_val_not_digit:
      printf("The character following '#' in '%s' neither a digit not a '+' or '-' sign\n", state->buffer);
      break;
   case e43_inval_indirect_reg:
      printf("Expected a register identifier 'rx' after the '*' character\n");
      break;

   case e46_imm_inv_after_pm:
      printf("The character following the sign character in '%s' neither a digit not a valid digit\n", state->buffer);
      break;

   case e44_indirect_reg_number_not_in_range:
      printf("The character following '*r' in '%s' is not a digit between 0 and 7\n", state->buffer);
      break;

   case e47_ext_chars_after_indirect_reg:
      printf("Detected additional characters after '*rx' in '%s'\n", state->buffer);
      break;

   case e48_no_num_after_dir_reg:
      printf("Expected a register number after the 'r' character\n");
      break;

   case e49_ext_chars_after_direct_reg:
      printf("Detected additional characters after 'rx' in '%s'\n", state->buffer);
      break;

   case e50_direct_reg_num_not_in_range:
      printf("The character following 'r' in '%s' is not a digit between 0 and 7\n", state->buffer);
      break;

   case e51_unknown_label:
      printf("A label with the name '%s' is not declared as external and is not defined in the current file\n", state->tmp_arg);

   default:
      break;






   }






}
