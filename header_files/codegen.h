#ifndef CODEGEN_H
#define CODEGEN_H

#include "common.h"
#include "symbol_table.h"
#include "parser.h"
#include "error.h"
#include "util.h"
#include "macro.h"


/**
 * @brief Converts the pre-lexed fields of an assembly instruction into a binary format.
 *
 * This function processes the pre-lexed fields of an assembly instruction, converting them into a binary format.
 * It performs the following steps:
 * - Converts the command field to its binary representation.
 * - Allocates memory for the binary representation, including space for a flag.
 * - Handles commands that do not require further data processing.
 * - Processes and converts the data fields of the command, if applicable.
 *
 * @param pre_decoded The array containing the pre-lexed fields of the instruction.
 * @param pre_decoded_size The number of pre-lexed fields.
 * @param keyword_table The table containing keyword information.
 * @param key The key of the command in the keyword table.
 * @param _label_table The table containing label information.
 * @return A pointer to the binary representation of the instruction, or NULL if an error occurs.
 */
int *convert_pre_lexed_fields_to_binary(string *pre_decoded, int pre_decoded_size, keyword *keyword_table, int key, label_table *_label_table);

/**
 * @brief Processes the data fields of a pre-lexed instruction and converts them into their binary representation.
 *
 * This function processes the data fields of a pre-lexed instruction, converting them into binary format.
 * It handles both known and unknown numbers of data fields by delegating to appropriate helper functions.
 *
 * @param which_data Indicator of whether the number of arguments is known or unknown.
 * @param post_decoded The array containing the binary command.
 * @param pre_decoded The array containing the pre-lexed fields of the instruction.
 * @param pre_decoded_size The number of pre-lexed fields.
 * @param _label_table The table containing label information.
 * @return A pointer to the updated binary command array.
 */
int *convert_data_fields_to_binary(int which_data, int *post_decoded, string *pre_decoded, int pre_decoded_size, label_table *_label_table);

/**
 * @brief Processes the data fields of a pre-lexed instruction with an unknown number of arguments and converts them into binary representation.
 *
 * This function handles the conversion of data fields for instructions with an unknown number of arguments.
 * It processes `.data` and `.string` directives, converting the data fields into their binary representations.
 * For `.data` directives, it converts the data into an integer array.
 * For `.string` directives, it converts the string into its ASCII values.
 *
 * @param pre_decoded The array containing the pre-lexed fields of the instruction.
 * @return A pointer to the binary representation of the data fields, or NULL if an error occurs.
 */
int *convert_data_fields_to_binary_unknown_args(string *pre_decoded);



/**
 * @brief Processes the data fields of a pre-lexed instruction with a known number of arguments and converts them into binary representation.
 *
 * This function handles the conversion of data fields for instructions with a known number of arguments.
 * It processes the arguments based on their addressing methods, including special cases such as two registers.
 * The function performs the following steps:
 * - Maps the addressing methods for the arguments.
 * - Converts the arguments into their binary representation.
 * - Reallocates memory for the binary command as needed.
 *
 * @param pre_decoded The array containing the pre-lexed fields of the instruction.
 * @param pre_decoded_size The number of pre-lexed fields.
 * @param post_decoded The array containing the binary command.
 * @param _label_table The table containing label information.
 * @return A pointer to the updated binary command array.
 */
int *convert_data_fields_to_binary_known_args(string *pre_decoded, int pre_decoded_size, int *post_decoded, label_table *_label_table);

/**
 * @brief Converts a command involving two registers into its binary representation.
 *
 * This function processes a command that involves two registers, determining the register numbers
 * and constructing the binary representation of the command. The binary value is constructed by
 * shifting the register numbers and adding a constant value for alignment.
 *
 * @param source The source register.
 * @param dest The destination register.
 * @return The binary representation of the two-register command.
 */
int convert_two_registers_to_binary(char *source, char *dest);

/**
 * @brief Maps an argument's data to its binary representation based on the addressing method.
 *
 * This function converts an argument's data into its binary representation based on the specified addressing method.
 * It handles immediate values, labels, and register addressing methods by performing the following steps:
 * - For immediate values, converts the number after '#' to binary.
 * - For labels, retrieves the label key to map their address later.
 * - For registers, converts the register number to binary.
 *
 * @param str The argument data to convert.
 * @param addressing_method The addressing method to use for the conversion.
 * @param _label_table The table containing label information.
 * @param source_or_dest Indicates whether the argument is a source or destination.
 * @return The binary representation of the argument, or UNDEFINED if an error occurs.
 */
int convert_argument_to_binary(char *str, int addressing_method, label_table *_label_table, int source_or_dest);

/**
 * @brief Calculates the binary representation for addressing methods 2 and 3.
 *
 * This function processes a number and its position (source or destination) to calculate
 * the binary representation for addressing methods 2 and 3. It performs bitwise operations
 * to place the number in the appropriate bits and sets a specific bit for identification.
 *
 * @param number The number to convert.
 * @param pos The position of the number (SOURCE or DEST).
 * @return The binary representation of the addressing method.
 */
int calculate_addressing_method_bits(int number, int pos);




#endif