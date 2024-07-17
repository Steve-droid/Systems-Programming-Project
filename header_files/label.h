/**
 * @file label.h
 * @brief Header file for the label management component of the assembly code translator.
 *
 * This file contains the declarations and definitions for managing labels within the assembly code.
 * Labels are used to mark positions in the code and are essential for jump and branch instructions.
 *
 * The primary role of the label management component is to:
 * - Store and manage label definitions and their corresponding addresses.
 * - Resolve label references within the assembly code, replacing label keys with actual addresses.
 * - Ensure that label addresses are correctly calculated and updated based on their positions in the code.
 * - Handle both internal and external labels, including special processing for entry and extern labels.
 *
 * The label management component plays a crucial role in ensuring that the assembly code
 * is correctly translated, with all labels accurately resolved to their corresponding addresses.
 */

#ifndef LABEL_H
#define LABEL_H

#include "common.h"
#include "macro.h"
#include "keyword.h"
#include "label.h"
#include "util.h"


label *new_empty_label(label **new_label);
void label_update_fields(label **label, char *label_name, int line_counter, int address);
label *get_label(label_table *_lable_table, char *label_name);
void label_destroy(label **_label);


label_table *new_empty_label_table(label_table **new_label_table);
label_table *insert_label(label_table *table, label *_lable);
void print_label_table(label_table *_label_table);
void label_table_destroy(label_table **_label_table);

label_table *fill_label_table(char *am_filename, macro_table *m_table, keyword *keywords_table);
char *extract_label_name_from_instruction(char *instruction);
bool label_name_is_valid(label_table *_label_table, char *label_name, keyword *keywords_table, macro_table *m_table);
void replace_label_keys_with_addresses(int *decoded, label_table *_label_table);
void assign_label_addresses(int **decoded_table, label_table *_label_table);


#endif 
