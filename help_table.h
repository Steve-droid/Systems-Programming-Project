#ifndef HELP_FUNCTION_H
#define HELP_FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_table.h"

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

typedef struct{
    char name[MAX_LINE_LENGTH];
    int key;
    int line;
    int address;
    int size;
    int entry_or_extern;
}label;

typedef struct{
    char name[MAX_KEYWORD_LENGTH];
    int key;
    int length;
}keyword;

typedef struct{
    char name[MAX_MACRO_NAME];
    int size;
}macro_table;

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

typedef struct{
    char data[MAX_LINE_LENGTH];
}string;

/* Declarations*/
void initialize_char_array(char* char_array);
void remove_prefix_spaces(char* line);
char* pointer_after_label(char* line, label* label_table, int current_line);
int command_number_by_key(keyword* keyword_table, int key);
int map_addressing_method(char* str, label* label_table);
void print_pre_decoded(string* pre_decoded, int pre_decoded_size);
void int_to_binary_array(int num, int *arr, int from_cell, int to_cell);
int binary_array_to_int(int *array);
int* convert_to_1D(int** array2D);
void initialize_int_array(int* arr , int size);
void print_array_in_binary(int* arr);
void print_binary(int num);
void print_label_table(label* label_table);
keyword* fill_keywords_table();
int is_empty_line(char* str);
int char_to_int(char c);
void remove_commas_from_str(char* str);
void shift_left_str(char* str , int steps);
int set_bits_7_to_10(int num);
int set_bits_3_to_6(int num);
int* convert_to_int_array(char* str);



#endif //HELP_FUNCTION_H