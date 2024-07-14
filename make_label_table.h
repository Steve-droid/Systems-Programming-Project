#ifndef MMN14_LIBRARY_H
#define MMN14_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pre_assembler.h"


#define MAX_LINE_LENGTH 81
#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */
#define TRUE 1
#define FALSE 0
#define FIRST_KEY 32768 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define MAX_KEYWORD_LENGTH 8 /* .string + '\0' */
#define KEYWORD_TABLE_LENGTH 30 /* Amount of keywords (8 registers + 16 commands + 4 directives + 2 macro definition*/
#define MAX_MACRO_NAME 20

typedef struct {
    char name[MAX_LINE_LENGTH];
    int key;
    int line;
    int *address;
    int size;
}label;

typedef struct {
    char name[MAX_KEYWORD_LENGTH];
    int key;
    int length;
}keyword;

typedef struct {
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



/* Declarations */
label *fill_label_table(macro_table *table, keyword *keywords_table);
label *create_labels_table();
void increase_labels_table(label **table, size_t new_size);
void initialize_char_array(char *char_array);
int label_name_is_valid(label *label_table, char *label_name, int table_size, keyword *keywords_table, macro_table *macroTable);
void fill_label_data_exclude_address(label *any_label, char *line, int line_counter);
void remove_prefix_spaces(char *line);
void str_cpy_until_char(char *destination, const char *source, char x);
int label_name_is_in_table(label *label_table, char *label_name, int table_size);
int label_name_is_keyword(char *label_name, keyword *keywords_table);
keyword *fill_keywords_table();
int label_name_is_macro(char *label_name, macro_table *table);
void fill_table_size(label *label_table, int labels_counter);


#endif //MMN14_LIBRARY_H