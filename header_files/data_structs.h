#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>


#define MAX_MACRO_AMOUNT 100
#define MAX_LINE_LENGTH 80
#define INITIAL_MACRO_CAPACITY 3
#define INITIAL_MACRO_TABLE_CAPACITY 3
#define INITIAL_NUM_TOKENS 3
#define MIN_MACRO_AMOUNT 3
#define UNKNOWN_NUMBER_OF_ARGUMENTS -2
#define NONE -10
#define OUTPUT_COMMAND_LEN 15
#define OPCODE_LEN 4
#define ADDRESSING_METHOD_LEN 4
#define SOURCE 2
#define DEST 1
#define FIRST_ADDRESS 100
#define FIRST_KEY 1 /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define KEYWORD_TABLE_LENGTH 30 /* Amount of keywords (8 registers + 16 commands + 4 directives + 2 macro definition*/
#define MAX_KEYWORD_LENGTH 8 /* .string + '\0' */
#define OPERATION_KEYWORDS 16
#define REGISTER_KEYWORDS 8
#define DIRECTIVE_KEYWORDS 4
#define MACRO_KEYWORDS 2
#define MAX_LABEL_LENGTH 31 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */ /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define UNDEFINED -1
#define INCREMENT 1
#define GET -1
#define RESET -2
#define NEITHER_EXTERN_NOR_ENTRY -4
#define UNSET -1

/*---Enums---*/

typedef enum {
    false = 0, true
} bool;

typedef enum {
    success, failure
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
} opcode;

typedef enum register_name {
    UNDEFINED_REGISTER = -1, r0, r1, r2, r3, r4, r5, r6, r7
} register_name;

typedef struct {
    char name[MAX_KEYWORD_LENGTH];
    int key;
    int length;
}keyword;

typedef struct macro {
    char **lines;      /* Vector of strings- the lines that the macro expands to */
    char *name;         /* The name of the macro */
    int line_count;     /* Number of lines in the macro */
    int line_capacity;
}macro;

typedef struct macro_table {
    macro **macros;            /* Vector of macro pointers */
    int macro_count;           /* Number of macros in the table */
    int capacity;
}macro_table;


typedef struct label {
    char name[MAX_LABEL_LENGTH];
    int key;                   /* Unique key for each label */
    size_t instruction_line;   /* The line number of the instruction that the label is attached to */
    size_t address;            /* The address of the label */
    bool declared_as_entry;    /* True if the label is declared as an entry */
    bool declared_as_extern;   /* True if the label is declared as an extern */
    bool ignore;               /* True when label is declared than defined- allows to only create one of the two */
    bool missing_definition;   /* True if the label is declared but not yet defined */
}label;

typedef struct label_table {
    label **labels;             /* Vector of label pointers */
    size_t size;                /* Number of labels in the table */
    size_t capacity;            /* Capacity of the table */
}label_table;



/**
 *@brief Holds all the information needed create a sequence of binary words from a string representation of an instruction
 * variables:
 * @tokens: A vector of strings- the tokens that the instruction is made of
 * @num_tokens: The number of tokens in the instruction
 * @capacity: The capacity of the tokens vector
 * @num_words_to_generate: The number of binary words to generate for the instruction, range: 1-3
 * @opcode: The opcode of the instruction
 * @cmd_key: The key of the command in the keyword table
 * @label_key: Holds the key of the label that the instruction is defined with
 * @address: The address of the instruction
 * @line_number: The line number of the instruction
 *
 * The rest of the arguments are flags and parameters used when scanning each character of label and arguments
 * This allows the parser to directly generate the binary representation- all analysis is done by the lexer
 * The struct also holds a partial binary representation of the instruction
 */
typedef struct instruction {

    /* General instruction parameters */
    char **tokens;                 /* Vector of strings- the tokens that the instruction is made of */
    size_t num_tokens;             /* Number of tokens in the instruction */
    size_t capacity;               /* Capacity of the tokens vector */
    size_t num_words_to_generate;  /* Number of binary words to generate for the instruction, range: 1-3 */
    opcode opcode;                 /* The opcode of the instruction */
    int cmd_key;                   /* The key of the command in the keyword table */
    int label_key;                 /* Holds the key of the label that the instruction is defined with */
    int address;                   /* The address of the instruction */
    size_t line_number;            /* The line number of the instruction */


    /* Flags used when scanning each character of label and arguments */
    bool is_src_entry;
    bool is_src_extern;
    bool is_dest_entry;
    bool is_dest_extern;

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

/**
 *@brief The instruction image created by the assembler
 * The instruction image/table holds all non-directive instructions that are scanned by the lexer
 * An instruction is only added to the table if it is found to be valid by the syntax analysis process(lexer)
 * variables:
 * @inst_vec: A vector of instruction pointers
 * @num_instructions: The number of instructions in the table
 * @capacity: The capacity of the table
 * @IC: The instruction counter- holds the current address of the instruction
 * @DC: The data counter- holds the current address of the data
 */
typedef struct instruction_table {
    inst **inst_vec;
    size_t num_instructions;
    size_t capacity;
    size_t IC;
    size_t DC;
} inst_table;







/**
 *@brief The struct that holds the data image- the binary words that are generated from the '.data' and '.string' directives
 * The data image is being updated each time a '.data' or '.string' directive is scanned.
 * variables:
 * @num_words: The number of binary words in the data image
 * @binary_word_vec: A vector of the binary words
 * @names: A vector of directive names- used for memory safety and type checking
 * @num_names: The number of directive names in the vector
 */
typedef struct data_image {
    size_t num_words;
    uint16_t *binary_word_vec;
    char **names;
    int num_names;

} data_image;


/**
 *@brief The struct that holds the state of the syntax analysis process
 * This struct is used both when scanning labels in 'symbols.c' and when scanning arguments in 'lexer.c'
 * variables:
 * @inst: A pointer to the current instruction being scanned
 * @label: A pointer to the current label being scanned
 * @k_table: A pointer to the keyword table
 * @m_table: A pointer to the macro table
 * @l_table: A pointer to the label table
 * @buffer: A buffer to hold the instruction when scanning for labels and arguments
 * @buffer_without_offset: Used for memory safety when scanning for labels and arguments
 * @am_filename: The expanded assembly file name
 * @as_filename: The assembly file name
 *
 */
typedef struct syntax_state {

    /* Pointers to data structures */
    inst *_inst;          /* Pointer to the current instruction being scanned */
    label *_label;        /* Pointer to the current label being scanned */
    keyword *k_table;     /* Pointer to the keyword table */
    macro_table *m_table; /* Pointer to the macro table */
    label_table *l_table; /* Pointer to the label table */

    /* Buffers to hold instruction when scanning for labels and arguments */
    char *buffer;
    char *buffer_without_offset;

    /* Pointers to filenames */
    char *am_filename;
    char *as_filename;


    /* Temporary variables used when scanning for labels and arguments */
    char *tmp_arg;
    status extern_or_entry;
    int index;
    int cmd_key;
    int line_number;
    int tmp_len;
    int label_key;
    int arg_count;
    int tmp_address;

    /* Flags used to indicate '.data' and '.string' directives */
    bool is_data;
    bool is_string;


    /* Flags used when scanning each character of label and arguments */
    bool continue_reading;
    bool label_name_detected;
    bool comma;
    bool whitespace;
    bool null_terminator;
    bool new_line;
    bool minus_sign;
    bool plus_sign;
    bool end_of_argument_by_space;
    bool first_quatiotion_mark;
    bool last_quatiotion_mark;
    bool digit;

} syntax_state;



typedef struct {
    char *am_filename;
    char *as_filename;
    char *generic_filename;
    char *full_filename;
    char *tmp;

    int backup_fnames_count;
    int filename_count;
    int original_line_count;
    int cleaned_line_count;

}system_state;

/**
 *@brief A struct that holds all the filenames that are used in the program
 * variables:
 * @as: A vector of strings that holds the .as filenames
 * @am: A vector of strings that holds the .am filenames
 * @generic: A vector of strings that holds the generic filenames(without any extension)
 * @amount: The amount of filenames in the vectors- vector size is the same for all vectors
 */
typedef struct {
    char **generic;
    char **as;
    char **am;
    int *errors;
    size_t amount;
}filenames;



/* Create functions */
status create_macro(char *macro_name, macro **new_macro);
label *create_label();
status create_instruction(inst **_new_inst);
status create_empty_token(inst *_inst);
macro_table *create_macro_table(void);
label_table *create_label_table(label_table **new_label_table);
status create_instruction_table(inst_table **new_inst_table);

/* Insert functions */
status insert_line_to_macro(macro *mac, char *line);
status insert_macro_to_table(macro_table *table, macro *mac);
status insert_label(label_table *table, label **_label);
status insert_token_to_inst(inst *_inst, char *token);
status insert_inst_to_table(inst_table *_inst_table, inst *_inst);

/* Destroy functions */
void destroy_macro(macro **macro);
void destroy_label(label **_label);
void destroy_instruction(inst **_inst);
void destroy_keyword_table(keyword **_keyword_table);
void destroy_macro_table(macro_table **table);
void destroy_label_table(label_table **_label_table);
void destroy_instruction_table(inst_table **_inst_table);


/* Get functions */
macro *get_macro(macro_table *table, char *name);


#endif
