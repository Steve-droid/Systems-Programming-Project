#ifndef ASM_ERROR_H
#define ASM_ERROR_H

#include <stdarg.h>
#include "data_structs.h"
#include "symbols.h"
#include "file_util.h"


typedef enum {

   /* Memory handling errors */
   m1_general_memerr,
   m2_syntax_state,
   m3_inst_tab_creation,
   m4_inst_creation,
   m5_inst_insert,
   m6_tok_gen_mem,
   m7_generic_creation,
   m9_create_sys_state,
   m10_create_backup_filename,
   m11_create_keyword_table,
   m12_create_label_table,
   m13_create_label,
   m14_copy_label_name,
   m15_insert_label,
   m16_create_macro,
   m17_create_macro_table,
   m18_create_instruction,
   m19_create_inst_table,
   m20_create_token,
   m21_insert_macro_to_table,
   m22_insert_line_to_macro,





   /* File handling errors */
   f1_file_open,
   f2_as_creation,
   f3_backup,
   f4_am_creation,
   f5_tmp_file,
   f6_open_tmpfile,
   f7_write_to_tmp_file,
   f8_line_mismatch,
   f9_rmv_original,
   f10_rename_tmp,
   f11_bckup_null,

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
   ex28_inval_method_mov,
   ex28_inval_method_add,
   ex28_inval_method_sub,

   e29_inval_method_imm_lea,

   e30_ext_arg_clr_not_inc_dec_red,
   ex30_ext_arg_clr,
   ex30_ext_arg_not,
   ex30_ext_arg_inc,
   ex30_ext_arg_dec,
   ex30_ext_arg_red,

   e31_mult_assign_clr_not_inc_dec_red,
   ex31_mult_assign_clr,
   ex31_mult_assign_not,
   ex31_mult_assign_inc,
   ex31_mult_assign_dec,
   ex31_mult_assign_red,
   e32_imm_clr_not_inc_dec_red,
   ex32_imm_clr,
   ex32_imm_not,
   ex32_imm_inc,
   ex32_imm_dec,
   ex32_imm_red,
   e33_tomany_jmp_bne_jsr,
   ex33_toomany_jmp,
   ex33_toomany_bne,
   ex33_toomany_jsr,
   e34_mul_assign_jmp_bne_jsr,
   ex34_mul_assign_jmp,
   ex34_mul_assign_bne,
   ex34_mul_assign_jsr,
   e35_imm_jmp_bne_jsr,
   ex35_imm_jmp,
   ex35_imm_bne,
   ex35_imm_jsr,
   e36_dir_jmp_bne_jsr,
   ex36_dir_jmp,
   ex36_dir_bne,
   ex36_dir_jsr,
   e37_toomany_prn,
   e38_addr_mult_assign_prn,
   e39_cmp_extra_args,
   e40_toomany_rts_stop,
   ex40_toomany_rts,
   ex40_toomany_stop,
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
   e53_ext_chars_after_endmacr,
   e54_macr_already_defined,
   e55_macro_not_found,
   e56_macro_name_not_valid,
   e57_macroname_same_as_keyword,
   e58_cannot_insert_macro,
   e59_label_redef,
   e60_label_name_is_too_long,
   e61_label_name_is_keyword,
   e62_label_name_is_macro,
   e63_label_name_not_alphanumeric,
   e64_whitespace_between_label_and_colon,
   e66_redef_directive,
   e67_using_undefined_label,


   w1_label_declared_not_defined,



   /* Notes  */
   n1_prefix_op

} error_code;

/**
 *@brief This function calls the appropriate memory handling function to reset the data structures before moving to the next file.
 *
 * @param file_amount The number of files to process
 * @param fnames A data structure containing the names of the files to process
 * @param _macro_table A pointer to the macro table
 * @param keyword_table A pointer to the keyword table
 * @param _label_table A pointer to the label table
 * @param _instruction_table A pointer to the instruction table
 */
void reset_main(int file_amount, filenames **fnames, macro_table **_macro_table, keyword **keyword_table, label_table **_label_table, inst_table **_instruction_table);

/**
 *@brief This function calls the appropriate memory handling function to quit the process of file processing in case of a memory error.
 *
 *
 * @param fnames A pointer to the filenames data structure
 * @param _inst_table A pointer to the instruction table
 * @param am_file_ptr A pointer to the assembly file
 */
void quit_filename_creation(filenames **fnames);

/**
 *@brief This function calls the appropriate memory handling function to quit the process of lexical analysis in case of a memory error.
 *
 * @param state The syntax state object to destroy
 * @param _inst_table The instruction table to destroy
 * @param am_file_ptr The assembly file pointer to close
 */
void quit_lex(syntax_state **state, inst_table **_inst_table, FILE *am_file_ptr);

/**
 *@brief This function calls the appropriate memory handling function to quit the process of pre-assembling in case of a memory error.
 *
 * @param state The syntax state object to destroy
 * @param _macro_table The macro table to destroy
 * @param am_file_ptr The assembly file pointer to close
 * @param as_file_ptr The assembler file pointer to close
 */
void quit_pre_assembler(syntax_state **state, macro_table **_macro_table, FILE *am_file_ptr, FILE *as_file_ptr);

/**
 *@brief This function calls the appropriate memory handling function to quit the process of label parsing in case of a memory error.
 *
 * @param _label_table The label table to destroy
 * @param state The syntax state object to destroy
 * @param am_file_ptr The assembly file pointer to close
 */
void quit_label_parsing(label_table **_label_table, syntax_state **state, FILE *am_file_ptr);


/**
 *@brief This function prints an error message to the console in case of a file handling or memory error.
 *
 * @param sys_state The system state object containing necessary information about the error
 * @param syn_state The syntax state object containing necessary information about the error(used in case of a mix of system and syntax errors)
 * @param e_code The error code used to determine the error message to print
 */
void print_system_error(system_state *sys_state, syntax_state *syn_state, error_code e_code);

/**
 *@brief This function prints an error message to the console in case of a syntax error.
 *
 * @param state The syntax state object containing necessary information about the error
 * @param e_code The error code used to determine the error message to print
 */
void print_syntax_error(syntax_state *state, error_code e_code);





#endif
