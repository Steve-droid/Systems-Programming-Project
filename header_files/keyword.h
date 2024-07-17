/**
 *@file keyword.h
 * @brief This file contains the keyword table and keyword functions
 * The keyword table consists of all the keywords in the assembly language.
 * The keyword table is used to check if a given word in the assembly code is a keyword.
 */
#ifndef KEYWORD_H
#define KEYWORD_H

#include "common.h"
#include "util.h"



/**
 *@brief Creates a table of keywords
 * The function creates a table of keywords and their corresponding keys.
 * @return keyword*
 */
keyword *fill_keyword_table();

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
 * @brief Identifies and returns the keyword key for a command in the given line of assembly code.
 *
 * This function processes a line of assembly code to identify the command present in the line.
 * It performs the following steps:
 * - Skips any labels and leading spaces in the line.
 * - Extracts the command name from the line.
 * - Compares the extracted command name against the keywords in the keyword table.
 * - Returns the keyword key if a match is found, otherwise returns UNDEFINED.
 *
 * @param line The line of assembly code to process.
 * @param _label_table The table containing label information.
 * @param keyword_table The table containing keyword information.
 * @param current_line The current line number being processed.
 * @return The keyword key if the command is found, otherwise UNDEFINED.
 */
int identify_command(char *line, label_table *_label_table, keyword *keyword_table, int current_line);

/**
 * @brief Retrieves the opcode value for a given command key from the keyword table.
 *
 * This function maps a command key to its corresponding opcode value using the keyword table.
 * It returns the opcode value for the command based on predefined opcode mappings.
 *
 * @param keyword_table The table containing keyword information.
 * @param key The key of the command to look up.
 * @return The opcode value for the command, or error codes like NO_NEED_TO_DECODE or UNKNOWN_NUMBER_OF_ARGUMENTS.
 */
int get_command_opcode(keyword *keyword_table, int key);



#endif 