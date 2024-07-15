#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#define _POSIX_C_SOURCE 200809L
#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */
#define FIRST_KEY 40001 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define MAX_LINE_LENGTH 81
#define TRUE 1
#define FALSE 0
#define MAX_KEYWORD_LENGTH 8 /* .string + '\0' */
#define KEYWORD_TABLE_LENGTH 30 /* Amount of keywords (8 registers + 16 commands + 4 directives + 2 macro definition*/
#define MAX_MACRO_NAME 20
#define UNDEFINED -1
#define FLAG -50000
#define PLUS -50001
#define MINUS -50002
#define COMMA -50003
#define UNUSED(x) (void)(x)


typedef enum {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_ERROR_OPEN_SRC,
    STATUS_ERROR_OPEN_DEST,
    STATUS_ERROR_READ,
    STATUS_ERROR_WRITE,
    STATUS_ERROR_INVALID_EXTENSION,
    STATUS_ERROR_MACRO_REDEFINITION,
    STATUS_ERROR_MEMORY_ALLOCATION,
    STATUS_ERROR_MACRO_NOT_FOUND,
    STATUS_ERROR_MACRO_TABLE_IS_EMPTY,
    STATUS_ERROR_MACRO_EXPANDS_TO_NOTHING,
    STATUS_ERROR_WHILE_CREATING_FILENAME
} status;

typedef struct {
    char name[MAX_LINE_LENGTH];
    int key;
    int line;
    int address;
    int size;
    int entry_or_extern;
}label;

typedef struct {
    char name[MAX_KEYWORD_LENGTH];
    int key;
    int length;
}keyword;

typedef enum {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    STOP,
    MACR,
    ENDMACR,
    DATA,
    STRING,
    ENTRY,
    EXTERN
} keyword_name;

typedef enum {
    MOV_OPCODE,    /* 0 */
    CMP_OPCODE,    /* 1 */
    ADD_OPCODE,    /* 2 */
    SUB_OPCODE,    /* 3 */
    LEA_OPCODE,    /* 4 */
    CLR_OPCODE,    /* 5 */
    NOT_OPCODE,    /* 6 */
    INC_OPCODE,    /* 7 */
    DEC_OPCODE,    /* 8 */
    JMP_OPCODE,    /* 9 */
    BNE_OPCODE,    /* 10 */
    RED_OPCODE,    /* 11 */
    PRN_OPCODE,    /* 12 */
    JSR_OPCODE,    /* 13 */
    RTS_OPCODE,    /* 14 */
    STOP_OPCODE,   /* 15 */
} commands_opcodes;

typedef struct {
    char data[MAX_LINE_LENGTH];
}string;


/* File Utilities */
char *create_file_name(char *initial_name, char *extension);
status remove_extension(char *full_filename);
status copy_file_contents(char *src_filename, char *dest_filename);
status remove_whitespace(char *filename);
status backup_files(char ***backup_filenames, int file_count, char *filenames[]);
status initallize_file_names(char *filename, char **am_filename, char **as_filename);
void delete_backup_names(size_t num_files, char **backup_names);

/* Other Utilities */
label *create_labels_table();
void fill_label_data_exclude_address(label *any_label, char *line, int line_counter);
label *fill_label_table(char *am_filename, macro_table *macroTable, keyword *keywords_table);
void fill_table_size(label *label_table, int labels_counter);
void increase_labels_table(label **table, size_t new_size);
int label_name_is_in_table(label *label_table, char *label_name, int table_size);
int label_name_is_keyword(char *label_name, keyword *keywords_table);
int label_name_is_macro(char *label_name, macro_table *table);
int label_name_is_valid(label *label_table, char *label_name, int table_size, keyword *keywords_table, macro_table *macroTable);
void str_cpy_until_char(char *destination, const char *source, char x);
int string_to_label(char *str, label *label_table);


#endif 