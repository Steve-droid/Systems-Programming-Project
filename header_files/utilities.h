#ifndef UTILITIES_H
#define UTILITIES_H
#define _POSIX_C_SOURCE 200809L


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include "macro.h"
#include "symbol_table.h"

#define _POSIX_C_SOURCE 200809L
#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */
#define FIRST_KEY 40001 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define TRUE 1
#define FALSE 0
#define KEYWORD_TABLE_LENGTH 30 /* Amount of keywords (8 registers + 16 commands + 4 directives + 2 macro definition*/
#define OPERATION_KEYWORDS 16
#define REGISTER_KEYWORDS 8
#define DIRECTIVE_KEYWORDS 4
#define MACRO_KEYWORDS 2
#define REGISTER_LEN 2
#define MAX_MACRO_NAME 20
#define UNDEFINED -1
#define FLAG -50000
#define PLUS -50001
#define MINUS -50002
#define COMMA -50003
#define UNUSED(x) (void)(x)


typedef struct {
    char data[MAX_LINE_LENGTH];
}string;


/* File Utilities */
char *create_file_name(char *initial_name, char *extension);
status remove_file_extension(char *full_filename, char **generic_filename);
status copy_file_contents(char *src_filename, char *dest_filename);
status remove_whitespace(char *filename);
status duplicate_files(char ***backup_filenames, int file_count, char *filenames[], char *extention);
status delete_filenames(size_t file_amount, char **filenames);

/* Other Utilities */
void str_cpy_until_char(char *destination, const char *source, char x);
int string_to_label(char *str, label *label_table);
int command_number_by_key(keyword *keyword_table, int key);
int map_addressing_method(char *str, label *label_table);
int *convert_to_int_array(char *str);
void int_to_binary_array(int num, int *arr, int from_cell, int to_cell);
int binary_array_to_int(int *array);
void initialize_char_array(char *char_array);
int is_empty_line(char *str);
void print_2D_array(int **arr);
char *pointer_after_label(char *line, label *label_table, int current_line);
void remove_prefix_spaces(char *line);
void print_pre_decoded(string *pre_decoded, int pre_decoded_size);
void print_array_in_binary(int *arr);
int *convert_to_1D(int **array2D);
void print_binary(int num);
keyword *fill_keywords_table();
void insert_operation_keywords(keyword *keywords_table);
void print_label_table(label *label_table);


#endif 