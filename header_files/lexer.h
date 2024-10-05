#ifndef LEXER_H
#define LEXER_H

#include "data_structs.h"
#include "text_util.h"
#include "symbols.h"
#include "asm_error.h"



 /**
  * @brief Lexically analyze the assembly file
  * This function performs lexical analysis on the assembly file by calling the necessary helper functions.
  * The function returns a table of instructions represented as strings.
  * The instruction table returned by this function contains all necessary information to generate the machine code.
  * Generating the machine code is done in the next stage of the translation process- the parser.
  * @param am_filename The name of the assembly file after preprocessing
  * @param as_filename The name of the assembly file before preprocessing
  * @param _label_table The table of labels created from the assembly file
  * @param keyword_table The table of keywords and their corresponding keys
  * @param syntax_error_count The number of syntax errors encountered during lexical analysis
  * @return inst_table* The table of instructions created from the assembly file
  */
inst_table *lex(char *am_filename, char *as_filename, label_table *_label_table, keyword *keyword_table, int *syntax_error_count);

/**
 *@brief Update or get the data counter
 *
 * @param prompt A flag to indicate whether to update the data counter or get its value
 * @param amount The amount to update the data counter by
 * @return size_t The value of the data counter
 */
size_t DC(int prompt, size_t amount);

/**
 *@brief Update or get the instruction counter
 *
 * @param prompt A flag to indicate whether to update the instruction counter or get its value
 * @param amount The amount to update the instruction counter by
 * @return size_t The value of the instruction counter
 */
size_t IC(int prompt, size_t amount);

/**
 *@brief Generate string tokens from the assembly file
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @param keyword_table The table of keywords and their corresponding keys
 * @param _label_table The table of labels created from the assembly file
 * @return status A status code indicating the success of the operation
 */
status generate_tokens(syntax_state *state, keyword *keyword_table, label_table *_label_table);

/**
 *@brief Handle an instruction that contains a .data or .string directive
 * This functions calls the necessary helper functions to process the data or string command.
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @param _label_table The table of labels created from the assembly file
 * @param keyword_table The table of keywords and their corresponding keys
 * @return status A status code indicating the success of the operation
 */
status assign_data(syntax_state *state, label_table *_label_table, keyword *keyword_table);

/**
 *@brief Handle an instruction that contains arguments(operands)
 *  This functions calls the necessary helper functions to process the arguments of an instruction.
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @param _label_table The table of labels created from the assembly file
 * @param keyword_table The table of keywords and their corresponding keys
 * @return status A status code indicating the success of the operation
 */
status assign_args(syntax_state *state, label_table *_label_table, keyword *keyword_table);

/**
 *@brief A helper function to process an instruction that contains a .data directive
 *
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @param _label_table The table of labels created from the assembly file
 * @return status A status code indicating the success of the operation
 */
status process_data_command(syntax_state *state, label_table *_label_table);

/**
 *@brief A helper function to process an instruction that contains a .string directive
 *
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @param _label_table The table of labels created from the assembly file
 * @return status A status code indicating the success of the operation
 */
status process_string_command(syntax_state *state, label_table *_label_table);

/**
 *@brief A function that validated and assigns addressing methods for the arguments of an instruction
 * This function checks that each operation has the correct number of arguments and assigns the addressing method to each argument.
 * It is also responsible for handling errors related to the addressing methods of the arguments by calling the appropriate error handling functions.
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @param argument The argument to assign the addressing method to
 * @param _label_table The table of labels created from the assembly file
 * @param keyword_table The table of keywords and their corresponding keys
 * @return status A status code indicating the success of the operation
 */
status assign_addressing_method(syntax_state *state, char *argument, label_table *_label_table, keyword *keyword_table);

/**
 *@brief A function that validates the data members of an instruction
 * This function checks that the arguments passed through a '.data' directive are valid.
 * This function is a helper function for the 'process_data_command' function.
 * @param state The syntax state of the current instruction as defined in data_structs.h
 * @return status A status code indicating the success of the operation
 */
status validate_data_members(syntax_state *state);

/**
 *@brief This function assigns addresses to each instruction in the instruction table
 * This function is called after the instruction table has been filled with valid instructions.
 * The address assignment is done according to the number of binary words each string instruction translates to.
 *
 * @param _inst_table The instruction table to assign addresses to
 * @param _label_table The table of labels created from the assembly file
 * @param _keyword_table The table of keywords and their corresponding keys
 * @return status A status code indicating the success of the operation
 */
status assign_addresses(inst_table *_inst_table, label_table *_label_table, keyword *_keyword_table);

#endif 
