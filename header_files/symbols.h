

#ifndef SYMBOLS_H
#define SYMBOLS_H
#include "common.h"
#include "text_util.h"
#include "macro.h"


/**
 *@brief Creates a table of keywords
 * The function creates a table of keywords and their corresponding keys, defined as enum in common.h.
 * The table is used to identify commands and registers in the assembly file.
 * @return keyword*
 */
keyword *fill_keyword_table();

/**
 *@brief Creates a table of labels
 * This function parses the assembly file and creates a table of labels.
 *
 * @param am_filename The name of the assembly file after preprocessing
 * @param m_table The macro table containing macro definitions
 * @param keywords_table The table of keywords and their corresponding keys
 * @return label_table* The table of labels created from the assembly file
 */
label_table *fill_label_table(char *am_filename, macro_table *m_table, keyword *keywords_table);

/**
 *@brief Get a keyword by its name
 * This function searches for a keyword in the keyword table by its name.
 * @param keywords_table The table of keywords to search in
 * @param name The name of the keyword to search for
 * @return keyword* The keyword if found, or NULL if not found
 */
keyword *get_keyword_by_name(keyword *keywords_table, char *name);

/**
 *@brief Get a label by its name
 * This function searches for a label in the label table by its name.
 * Each label has a unique name that is used to identify it.
 * @param _label_table The table of labels to search in
 * @param label_name The name of the label to search for
 * @return label* The label if found, or NULL if not found
 */
label *get_label_by_name(label_table *_lable_table, char *label_name);

/**
 *@brief Get a keyword by its key
 * This function searches for a keyword in the keyword table by its key.
 * @param keywords_table The table of keywords to search in
 * @param key The key of the keyword to search for
 * @return keyword* The keyword if found, or NULL if not found
 */
keyword *get_keyword_by_key(keyword *keywords_table, int key);

/**
 *@brief Get a label by its key
 * This function searches for a label in the label table by its key.
 * Each label has a unique key that is used to identify it.
 * @param _label_table The table of labels to search in
 * @param key The key of the label to search for
 * @return label* The label if found, or NULL if not found
 */
label *get_label_by_key(label_table *_label_table, int key);

/**
 * @brief Determines the number of arguments required for a given command.
 *
 * This function takes a command name as input and returns the number of arguments that
 * the command requires. The number of arguments is based on predefined command names.
 *
 * @param command_name The name of the command to check.
 * @return The number of arguments required for the command, or special constants like UNKNOWN_NUMBER_OF_ARGUMENTS or UNDEFINED.
 */
int get_command_argument_count(int command_key);


/**
 *@brief Get the addressing method that matches a given argument in an instruction
 * This function is used by the lexer to determine the addressing method of an argument in an instruction.
 * @param state The syntax state of the current instruction as defined in text_util.h
 * @param sub_inst The argument to check- a substring of the instruction
 * @param _label_table A table of labels to check if the argument is a label
 * @return addressing_method The addressing method of the argument, as defined in common.h
 */
addressing_method get_addressing_method(syntax_state *state, char *sub_inst, label_table *_label_table);

/**
 *@brief
 *
 * @param state
 * @param _label_table
 * @param keyword_table
 * @return keyword_name
 */
keyword_name identify_command(syntax_state *state, label_table *_label_table, keyword *keyword_table);


/**
 *@brief Get the opcode of a command by its keyword
 * This function searches for a command in the keyword table by its key and returns the opcode of the command.
 * @param keyword_table The table of keywords to search in
 * @param key The key of the keyword to search for
 * @return opcode The opcode of the command if found, or -1(UNDIFINED) if not found
 */
opcode get_command_opcode(keyword *keyword_table, int key);

/**
 *@brief Update the fields of a label
 * This function updates the fields of a label with the given values.
 * @param new_label A pointer to the label to update
 * @param label_name A string containing the name of the label
 * @param line_counter An integer representing the line number of the label in the assembly file
 * @param _entry_or_external An enum representing the status of the label (entry or external)
 * @param address The memory address of the label
 */
void label_update_fields(label **new_label, char *label_name, int line_counter, status _entry_or_external, int address);


/**
 *@brief Get the number of a command by its keyword
 * This function searches for a command in the keyword table by its key and returns the number of the command.
 * @param keyword_table The table of keywords to search in
 * @param key The key of the keyword to search for
 * @return int The number of the command if found, or -1(UNDIFINED) if not found
 */
int command_number_by_key(keyword *keyword_table, int key);

/**
 *@brief Extract the label name from an instruction
 * This function extracts the string before a ':' character in an instruction, which is the label name.
 * @param _buffer A pointer to the buffer containing the instruction
 * @param _entry_or_external A pointer to the status of the label (entry or external) to update
 * @return char* A string containing the label name
 */
char *extract_label_name_from_instruction(char **_buffer, status *_entry_or_external);

/**
 *@brief Get the register number by its name
 * This function searches extracts the register number from a string containing the register name.
 * @param register_as_string A string containing the register name
 * @return register_name The register number if found, or -1 if not found
 */
register_name get_register_number(char *register_as_string);


/**
 *@brief Check if a label name is valid
 * This function checks if a label name is valid according to the rules of the assembly language.
 * @param _label_table A table of labels to check if the label name is unique
 * @param _buffer A pointer to the buffer containing the label name
 * @param keywords_table A table of keywords to check if the label name is a keyword
 * @param _macro_table A table of macros to check if the label name is a macro
 * @param entry_or_ext A pointer to the status of the label (entry or external) to update
 * @return status The status of the validation operation
 */
validation_state label_name_is_valid(label_table *_label_table, char *_buffer, keyword *keywords_table, macro_table *_macro_table, status *entry_or_ext);


/**
 *@brief Create a new label object
 *
 * @param new_label A pointer to the label to create
 * @return label* A pointer to the new label object
 */
label *new_empty_label(label **new_label);

/**
 *@brief Create a new label table
 *
 * @param new_label_table A pointer to the label table to create
 * @return label_table* A pointer to the new label table object
 */
label_table *new_empty_label_table(label_table **new_label_table);

/**
 *@brief Insert a label into a label table
 * This function inserts a label into a label table.
 * @param table A pointer to the label table to insert the label into
 * @param _label A pointer to the label to insert
 * @return status The status of the insertion operation
 */
status insert_label(label_table *table, label **_label);


/**
 *@brief Destroy a label table
 *
 * @param _label_table The label table to destroy
 */
void destroy_label_table(label_table **_label_table);

/**
 *@brief Destroy a label
 *
 * @param _label The label to destroy
 */
void destroy_label(label **_label);

/**
 *@brief Destroy a keyword table
 *
 * @param _keyword_table The keyword table to destroy
 */
void destroy_keyword_table(keyword **_keyword_table);

/**
 *@brief Print a label table
 * This function prints the contents of a label table to the console.
 * @param _label_table The label table to print
 */
void print_label_table(label_table *_label_table);
#endif 
