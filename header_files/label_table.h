#ifndef LABEL_TABLE_H
#define LABEL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "macro.h"

#define MAX_KEYWORD_LENGTH 8 /* .string + '\0' */
#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */
#define FIRST_KEY 40001 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */


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

typedef struct label {
    char name[MAX_LINE_LENGTH];
    size_t key;
    size_t line;
    size_t address;
    size_t size;
    int entry_or_extern;
}label;

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
    STOP_OPCODE   /* 15 */
} commands_opcodes;


/* Declarations */
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



#endif /* LABEL_TABLE_H */
