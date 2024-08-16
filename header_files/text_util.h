#ifndef TEXT_TOOLS_H
#define TEXT_TOOLS_H

#include "data_structs.h"
#include "text_util.h"
#include "symbols.h"
#include "lexer.h"
#include "instruction.h"


/**
 *@brief Create a syntax state object- a struct that holds the current state of the syntax analysis in 'symbols.c' and 'lexer.c'
 * This function creates a new syntax state object and initializes its fields.
 * @return syntax_state*
 */
syntax_state *create_syntax_state(void);

/**
 *@brief Reset the syntax state object
 * This function resets the syntax state object by clearing the buffer and setting the index to -1.
 * It also resets the flags that indicate the presence of certain characters in the buffer.
 * We reset the state each time we start processing a new instruction.
 * @param state The syntax state object to reset
 */
void reset_syntax_state(syntax_state *state);

/**
 *@brief Destroy the syntax state object
 * This function frees the memory allocated for the syntax state object.
 * @param state The syntax state object to destroy
 */
void destroy_syntax_state(syntax_state **state);

/**
 *@brief Updates the syntax state according to the command key
 *
 * @param state The syntax state of the current instruction as defined in ds.h
 * @param keyword_table
 * @param command_key The key of the command to update the syntax state according to
 */
void update_command(syntax_state *state, keyword *keyword_table, int command_key);

/**
 *@brief Continue reading the instruction buffer
 * This function checks if there are more characters to read in the instruction buffer.
 * @param instruction_buffer The buffer containing the instruction
 * @param state The syntax state of the current instruction as defined in ds.h
 * @return status The status of the operation
 */
bool continue_reading(char *instruction_buffer, syntax_state *state);


/*Operations on characters and pointers*/

/**
 *@brief Trim whitespace from a string
 * This function removes leading and trailing whitespace characters from a string.
 * @param str The string to trim
 * @return char* A pointer to the trimmed string
 */
char *trim_whitespace(char *str);

/**
 *@brief Skip the label name in an instruction
 * This function skips the label name in an instruction by offsetting the buffer pointer.
 * @param state The syntax state of the current instruction as defined in ds.h
 * @param _label_table A pointer to the label table
 */
void skip_label_name(syntax_state *state, label_table *_label_table);

/**
 *@brief Skip the .entry or .extern directive in an instruction
 * This function skips the .entry or .extern directive in an instruction by offsetting the buffer pointer.
 * @param _buffer A pointer to the buffer containing the instruction
 * @return char* A pointer to the buffer after skipping the directive
 */
char *skip_ent_or_ext(char *_buffer);


/**
 *@brief Check if a line is empty
 * This function checks if a line is empty by checking if it contains only whitespace characters.
 * @param str The line to check
 * @return bool The status of the check
 */
bool is_empty_line(char *str);

/**
 *@brief Duplicate a string
 * This function duplicates a string by allocating memory for a new string and copying the contents of the original string.
 * We use this function to create a copy of a string that we can modify without affecting the original string.
 * @param s The string to duplicate
 * @return char* The duplicated string
 */
char *my_strdup(char *s);

/* For debugging */
void print_bits(unsigned value, int num_bits);
void print_binary_to_file(uint16_t num, FILE *file_ptr);
#endif
