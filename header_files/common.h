#ifndef COMMON_H
#define COMMON_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <err.h>

#define MAX_LINE_LENGTH 80
#define UNKNOWN_NUMBER_OF_ARGUMENTS -2
#define NO_NEED_TO_DECODE -10
#define OUTPUT_COMMAND_LEN 15
#define OPCODE_LEN 4
#define ADDRESSING_METHOD_LEN 4
#define A 12
#define R 13
#define E 14 
#define SOURCE 2
#define DEST 1
#define FIRST_ADDRESS 100
#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */
#define FIRST_KEY 1 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define KEYWORD_TABLE_LENGTH 30 /* Amount of keywords (8 registers + 16 commands + 4 directives + 2 macro definition*/
#define MAX_KEYWORD_LENGTH 8 /* .string + '\0' */
#define OPERATION_KEYWORDS 16
#define REGISTER_KEYWORDS 8
#define DIRECTIVE_KEYWORDS 4
#define MACRO_KEYWORDS 2
#define REGISTER_LEN 2
#define MAX_NUMBER_OF_ARGUMENTS 2

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
    STATUS_ERROR_WHILE_CREATING_FILENAME,
    ERROR_NULL_TABLE,
    ERROR_NULL_INSTRUCTION
} status;

typedef enum addressing_method {
    UNDEFINED_METHOD = -1, IMMEDIATE, DIRECT, INDIRECT_REGISTER, DIRECT_REGISTER
} addressing_method;


typedef enum {
    UNDEFINED_KEYWORD = -1,
    R0, R1, R2, R3, R4, R5, R6, R7,                                                  /* Registers */
    MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP, /* Operations */
    MACR, ENDMACR,                                                                   /* Macros */
    DATA, STRING, ENTRY, EXTERN                                                      /* Directives */
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
    STOP_OPCODE   /* 15 */
} commands_opcodes;

typedef enum register_name {
    UNDEFINED_REGISTER = -1, r0, r1, r2, r3, r4, r5, r6, r7
} register_name;

typedef struct buffer_data {
    char *buffer;
    int index;
    int line_counter;
    int command_key;
} buffer_data;

typedef struct {
    char name[MAX_KEYWORD_LENGTH];
    int key;
    int length;
}keyword;

typedef struct instruction {
    char **tokens;
    int cmd_key;
    size_t num_tokens;
    size_t capacity;
    size_t line_number;
    bool is_dot_data;
    bool is_dot_string;
    bool is_entry;
    bool is_extern;
} inst;
typedef struct instruction_table {
    inst **inst_vec;
    size_t num_instructions;
    size_t capacity;
} inst_table;


typedef struct binary_word {
    int *bits_vec;
    bool first_in_instruction;
}bin_word;

typedef struct bin_instruction {
    bin_word **words;
    size_t num_words;
    size_t capacity;
    int cmd_key;
}bin_inst;

typedef struct bin_instruction_table {
    bin_inst **bin_inst_vec;
    size_t num_inst;
    size_t capacity;
}bin_table;

typedef struct {
    char data[MAX_LINE_LENGTH];
}string;

typedef struct label {
    char name[MAX_LABEL_LENGTH];
    size_t key;
    size_t instruction_line;
    size_t address;
    size_t size;
    bool is_entry;
    bool is_extern;
}label;

typedef struct label_table {
    label **labels;
    size_t size;
    size_t capacity;
}label_table;


typedef struct data_image {
    int **integer_data;
    char **string_data;
    size_t size;
    size_t capacity;
}data_image;

#endif