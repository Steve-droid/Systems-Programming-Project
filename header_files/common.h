/**
 *@file common.h
 * @brief Contains the common data structures and constants used throughout the assembler.
 * @date 2024-07-21
 *
 *
 */
#ifndef COMMON_H
#define COMMON_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint-gcc.h>

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
#define MAX_LABEL_LENGTH 33 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */ /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define MAX_MACRO_NAME 20
#define UNDEFINED -1
#define ERR -1
#define FLAG -50000
#define PLUS -50001
#define MINUS -50002
#define COMMA -50003

 /*---Enums---*/
typedef enum {
    invalid, valid
} validation_state;

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
    ERROR_NULL_INSTRUCTION,
    ERROR_NULL_LABEL,
    VALID_LABEL,
    INVALID_LABEL,
    CONTAINS_EXTERN,
    CONTAINS_ENTRY,
    CONTAINS_EXTERN_AND_ENTRY,
    NEITHER_EXTERN_NOR_ENTRY
} status;


typedef enum addressing_method {
    NO_ADDRESSING_METHOD = -2, UNDEFINED_METHOD = -1, IMMEDIATE = 0, DIRECT, INDIRECT_REGISTER, DIRECT_REGISTER
} addressing_method;


typedef enum {
    UNDEFINED_KEYWORD = -1,
    R0, R1, R2, R3, R4, R5, R6, R7,                                                  /* Registers */
    MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP, /* Operations */
    MACR, ENDMACR,                                                                   /* Macros */
    DATA, STRING, ENTRY, EXTERN                                                      /* Directives */
} keyword_name;


typedef enum {
    UNSET_OPCODE = -1,
    MOV_OPCODE = 0,    /* 0-> 0000 */
    CMP_OPCODE,    /* 1-> 0001 */
    ADD_OPCODE,    /* 2-> 0010 */
    SUB_OPCODE,    /* 3-> 0011 */
    LEA_OPCODE,    /* 4-> 0100 */
    CLR_OPCODE,    /* 5-> 0101 */
    NOT_OPCODE,    /* 6-> 0110 */
    INC_OPCODE,    /* 7-> 0111 */
    DEC_OPCODE,    /* 8-> 1000 */
    JMP_OPCODE,    /* 9-> 1001 */
    BNE_OPCODE,    /* 10->1010 */
    RED_OPCODE,    /* 11->1011 */
    PRN_OPCODE,    /* 12->1100 */
    JSR_OPCODE,    /* 13->1101 */
    RTS_OPCODE,    /* 14->1110 */
    STOP_OPCODE    /* 15->1111 */
} commands_opcodes;

typedef enum register_name {
    UNDEFINED_REGISTER = -1, r0, r1, r2, r3, r4, r5, r6, r7
} register_name;

/*---Data Structres---*/
typedef struct {
    char name[MAX_KEYWORD_LENGTH];
    int key;
    int length;
}keyword;

/**
 * struct instruction - Represents an assembly instruction or directive.
 *
 * @tokens: An array of strings representing tokens in the instruction.
 *          - For .data/.string directives:
 *            - tokens[0]: Directive name (e.g., ".data", ".string").
 *            - tokens[1]: Data as a single string (e.g., integers or characters).
 *          - For operations:
 *            - tokens[0]: Command name (e.g., add, mov).
 *            - tokens[1]: First argument, if it exists.
 *            - tokens[2]: Second argument, if it exists.
 * @cmd_key: An integer key representing the command (operation).
 * @label_key: An integer key representing the label.
 * @address: The memory address associated with this instruction.
 * @num_tokens: The number of tokens in the tokens array.
 * @capacity: The capacity of the tokens array (for dynamic memory management).
 * @line_number: The line number in the source code where this instruction appears.
 * @num_words_to_generate: The number of words this instruction will generate in machine code.
 * @num_dot_data_members: The number of members in a .data directive.
 * @num_dot_string_members: The number of characters in a .string directive.
 * @src_addressing_method: The addressing method for the source operand.
 * @dest_addressing_method: The addressing method for the destination operand.
 * @direct_label_name_src: The direct label name for the source operand (if used).
 * @direct_label_name_dest: The direct label name for the destination operand (if used).
 * @direct_label_key_src: The key for the direct label source operand.
 * @direct_label_key_dest: The key for the direct label destination operand.
 * @is_dot_data: A boolean indicating if this instruction is a .data directive.
 * @is_dot_string: A boolean indicating if this instruction is a .string directive.
 * @is_entry: A boolean indicating if this instruction is an entry point.
 * @is_extern: A boolean indicating if this instruction is an external reference.
 */
typedef struct instruction {

    /* General instruction parameters */
    char **tokens;
    size_t num_tokens;
    size_t capacity;
    size_t num_words_to_generate;
    commands_opcodes opcode;
    int cmd_key;
    int label_key;
    int address;
    size_t line_number;

    /* Directive parameters */
    size_t num_dot_data_members;
    size_t num_dot_string_members;
    bool is_dot_data;
    bool is_dot_string;
    bool is_entry;
    bool is_extern;

    /* Operations parameters */
    addressing_method src_addressing_method;
    addressing_method dest_addressing_method;


    /* For immediate addressing */
    int immediate_val_src;
    int immediate_val_dest;


    /* For direct addressing */
    int direct_label_key_src;
    int direct_label_key_dest;
    char direct_label_name_src[MAX_LINE_LENGTH];
    char direct_label_name_dest[MAX_LINE_LENGTH];
    int direct_label_address_src;
    int direct_label_address_dest;

    /* For indirect register addressing */
    int indirect_reg_num_src;
    int indirect_reg_num_dest;


    /* For direct register addressing */
    int direct_reg_num_src;
    int direct_reg_num_dest;


    /* Binary representation parameters */
    uint16_t *binary_word_vec;
    uint16_t bin_ARE;
    uint16_t bin_opcode;
    uint16_t bin_src_method;
    uint16_t bin_dest_method;
    uint16_t bin_address;

} inst;
typedef struct instruction_table {
    inst **inst_vec;
    size_t num_instructions;
    size_t capacity;
    size_t IC;
    size_t DC;
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


typedef struct label {
    char name[MAX_LABEL_LENGTH];
    int key;
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


size_t DC(char *prompt, size_t amount);
size_t IC(char *prompt, size_t amount);

typedef struct data_image {
    char **data;
    size_t num_words;
    char **dot_data;
    size_t num_dot_data;
    char **dot_string;
    size_t num_dot_string;
} data_image;

typedef struct syntax_state {
    validation_state _validation_state;
    status extern_or_entry;
    int index;
    int cmd_key;
    int line_number;
    inst *_inst;
    char *buffer;
    int label_key;

    bool continue_reading;
    bool label_name;
    bool comma;
    bool whitespace;
    bool null_terminator;
    bool new_line;
    bool minus_sign;
    bool plus_sign;
    bool end_of_argument_by_space;
    bool end_of_argument;
    bool end_of_string;
    bool first_quatiotion_mark;
    bool last_quatiotion_mark;
    bool digit;
    bool is_data;
    bool is_string;
    bool is_entry;
    bool is_extern;
} syntax_state;




#endif