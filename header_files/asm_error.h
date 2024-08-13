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
   m6_tok_gen_mem,
   m7_generic_creation,
   m8_rmv_ext,

   /* File handling errors */
   f1_file_open,
   f2_as_creation,
   f3_backup,
   f4_am_creation,
   f5_tmp_file,
   f6_open_tmp_file,
   f7_write_to_tmp_file,
   f8_line_mismatch,
   f9_rmv_original,
   f10_rename_tmp,
   
   /* General errors */
   e1_undef_cmd,
   e2_tokgen, 
   e3_addr_assign,
   e4_extra_comma,
   e5_missing_args,
   e6_arg_order,
   e7_lea_nondir_src,
   e8_lea_imm_dest,
   e9_inval_opcode,
   e10_series_of_commas,
   e11_missing_comma,
   e12_too_much_args,
   e13_comma_after_last_arg,
   e14_comma_before_data,
   e15_inval_data_symbol,
   e16_no_comma_betw_ints_data,
   e17_extre_comma_data_mid,
   e18_extra_comma_data_end,
   e19_pm_wrong_position,
   e20_pm_no_int,
   e21_str_no_qm,
   e22_str_no_qm_start,
   e23_str_no_qm_end,
   e24_inval_label_name,
   e25_ent_ex_args,
   e26_third_assignment,
   e27_inval_addr_mehod,
   e28_inval_method_mov_add_sub,
   e29_inval_method_imm_lea,
   e30_ext_arg_clr_not_inc_dec_red,
   e31_mult_assign_clr_not_inc_dec_red,
   e32_imm_clr_not_inc_dec_red,
   e33_tomany_jmp_bne_jsr,
   e34_mul_assign_jmp_bne_jsr,
   e35_imm_jmp_bne_jsr,
   e36_dir_jmp_bne_jsr,
   e37_toomany_prn,
   e38_addr_mult_assign_prn,
   e39_cmp_extra_args,
   e40_toomany_rts_stop,
   e41_lbl_arg,
   e42_imm_val_not_digit,
   e43_inval_indirect_reg,
   e44_indirect_reg_number_not_in_range,
   e45_inval_name_after_astr_r,
   e46_imm_inv_after_pm,
   e47_ext_chars_after_indirect_reg,
   e48_no_num_after_dir_reg,
   e49_ext_chars_after_direct_reg,
   e50_direct_reg_num_not_in_range,
   e51_unknown_label,
   e52_inval_ext,


   /* Notes  */
   n1_prefix_op

} error_code;

void quit_main(int file_amount, filenames **fnames, macro_table **_macro_table, keyword **keyword_table, label_table **_label_table, inst_table **_instruction_table);
void quit_filename_creation(filenames **fnames);
void quit_lex(syntax_state **state, inst_table **_inst_table, FILE *am_file_ptr);
void my_perror(syntax_state *state, error_code e_code);

#endif
