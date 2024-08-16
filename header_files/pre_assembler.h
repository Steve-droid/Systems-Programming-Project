#ifndef PRE_ASSEMBLER_H_
#define PRE_ASSEMBLER_H_

#include "data_structs.h"
#include "file_util.h"

#define MAX_MACRO_NAME_LENGTH 73
#define MAX_MACRO_AMOUNT 100
#define DEFINE_SEQUENCE_LEN 5

/**
 * @brief Check if the current line is a macro definition
 * This function checks if the current line is a macro definition.
 * A macro definition is a line that starts with 'macr '.
 * @param state The syntax state of the current instruction as defined in text_util.h
 * @return true if the line is a macro definition, false otherwise
 */
bool is_macro_definition(syntax_state *state);

/**
 * @brief Check if the current line is a macro call
 * This function checks if the current line is a macro call.
 * A macro call is a line that starts with a macro name.
 * @param line The line to check
 * @param table The macro table to search in
 * @return true if the line is a macro call, false otherwise
 */
bool is_macro_call(char *line, macro_table *table);

/**
 * @brief Process a new macro
 * This function processes a new macro definition.
 * The function reads the macro lines from the assembly file into the macro
 * It then adds the macro to the macro table.
 * @param state The syntax state of the current instruction as defined in text_util.h
 * @param macro_name The name of the macro
 * @param as_file The assembly file to read from
 * @param table The macro table to add the macro to
 * @return status The status of the operation
 */
status process_new_macro(syntax_state *state, char *macro_name, FILE *as_file, macro_table *table);

/**
 * @brief Expand a macro
 * This function expands a macro call into the lines of the macro definition
 * @param macro_name The name of the macro to expand
 * @param am_file The macro file to read from
 * @param table The macro table to search in
 * @return status The status of the operation
 */
status expand_macro(char *macro_name, FILE *am_file, macro_table *table);

/**
 * @brief Pre-assemble the assembly file
 * This function creates a '.am' file from the '.as' file.
 * The '.am' file contains the assembly file with the macros expanded.
 * @param as_filename The name of the assembly file
 * @param am_filename The name of the macro file
 * @param keyword_table The keyword table to use
 * @return macro_table* The macro table containing the macros defined in the assembly file
 */
macro_table *pre_assemble(char *as_filename, char *am_filename, keyword *keyword_table);




















#endif
