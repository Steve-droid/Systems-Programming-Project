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
#include "util.h"


 /**
  *@brief Creates a table of keywords
  * The function creates a table of keywords and their corresponding keys, defined as enum in common.h.
  * @return keyword*
  */
keyword *fill_keyword_table();

/**
 *@brief Creates a table of labels
 *
 * @return label_table*
 */
label_table *fill_label_table(char *am_filename, macro_table *m_table, keyword *keywords_table);

/**
 *@brief Get a keyword from the keyword table
 * The function receives a keyword table and a string.
 * The function searches for the string in the keyword table and returns the keyword if found.
 * If the string is not found in the keyword table, the function returns NULL.
 * @param keywords_table The keyword table
 * @param name The string to search for
 * @return keyword*
 */
keyword *get_keyword_by_name(keyword *keywords_table, char *name);

keyword *get_keyword_by_key(keyword *keywords_table, int key);

/**
 * @brief Determines the number of arguments required for a given command.
 *
 * This function takes a command name as input and returns the number of arguments that
 * the command requires. The number of arguments is based on predefined command names.
 *
 * @param command_name The name of the command to check.
 * @return The number of arguments required for the command, or special constants like UNKNOWN_NUMBER_OF_ARGUMENTS or UNDEFINED.
 */
int get_command_argument_count(int command_name);

/**
 * @brief Identifies a command keyword from a given string.
 *
 * This function takes a string as input and searches for a matching keyword in the keyword table.
 * If a match is found, the function returns the keyword. If no match is found, the function returns UNDEFINED_KEYWORD.
 *
 * @param _buffer_data The buffer data structure containing the current line of text.
 * @param _label_table The table containing label information.
 * @param keyword_table The table containing keyword information.
 * @return The keyword for the command, or UNDEFINED_KEYWORD if no match is found.
 */
keyword_name identify_command(buffer_data *_buffer_data, label_table *_label_table, keyword *keyword_table);
label *new_empty_label(label **new_label);
label *get_label(label_table *_lable_table, char *label_name);
label_table *new_empty_label_table(label_table **new_label_table);
label_table *insert_label(label_table *table, label *_lable);
void label_update_fields(label **new_label, char *label_name, int line_counter, status _entry_or_external, int address);
void print_label_table(label_table *_label_table);
int command_number_by_key(keyword *keyword_table, int key);
int get_command_opcode(keyword *keyword_table, int key);
char *extract_label_name_from_instruction(char *_buffer, status *_entry_or_external);
void label_table_destroy(label_table **_label_table);
void label_destroy(label **_label);
register_name get_register_number(char *register_as_string);
#endif 
