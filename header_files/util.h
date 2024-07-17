/**
 *@file general_utils.h
 * @brief This file contains the utilities functions and definitions
 * Utilities functions are functions that are used in multiple files and are not specific to any file
 * Includes functions for file manipulation, string manipulation, and other general functions
 * @version 0.1
 * @date 2024-07-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef UTIL_H
#define UTIL_H
#define _POSIX_C_SOURCE 200809L

#include "common.h"
#include "keyword.h"
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>


#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */ /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define MAX_MACRO_NAME 20
#define UNDEFINED -1
#define ERR -1
#define FLAG -50000
#define PLUS -50001
#define MINUS -50002
#define COMMA -50003
#define UNUSED(x) (void)(x)



 /* File Utilities */
char *create_file_name(char *initial_name, char *extension);
status remove_file_extension(char *full_filename, char **generic_filename);
status copy_file_contents(char *src_filename, char *dest_filename);
status remove_whitespace(char *filename);
status duplicate_files(char ***backup_filenames, int file_count, char *filenames[], char *extention);
status delete_filenames(size_t file_amount, char **filenames);

/* Other Utilities */
void str_cpy_until_char(char *destination, const char *source, char x);
int command_number_by_key(keyword *keyword_table, int key);
int map_addressing_method(char *str, label_table *_label_table);
int *convert_to_int_array(char *str);
void int_to_binary_array(int num, int *arr, int from_cell, int to_cell);
int binary_array_to_int(int *array);
void initialize_char_array(char *char_array);
bool is_empty_line(char *str);
void print_2D_array(int **arr);
char *pointer_after_label(char *line, label_table *_label_table, int current_line);
void remove_prefix_spaces(char *line);
void print_pre_decoded(string *pre_decoded, int pre_decoded_size);
void print_array_in_binary(int *arr);
int *convert_to_1D(int **array2D);
void print_binary(int num);
void print_label_table(label_table *_label_table);

/**
 * @brief Validates the format of a string representing a sequence of numbers for a .data directive.
 *
 * This function checks whether the provided string is a valid sequence of numbers for a .data directive.
 * It ensures that the numbers are properly formatted, with correct handling of signs (+/-) and commas.
 * The function performs the following checks:
 * - Ensures that numbers are properly separated by commas.
 * - Validates the use of plus and minus signs.
 * - Detects common formatting errors such as misplaced signs or commas.
 *
 * @param str The string to validate.
 * @return True if the format is valid, false otherwise.
 */
int validate_data_numbers(char *str);



#endif 