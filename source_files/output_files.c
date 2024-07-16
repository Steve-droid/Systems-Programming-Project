#include <stdio.h>
#include "utilities.h"
#include "macro.h"
#define WORD_LENGTH 15

typedef struct binary_word {
    int bits[WORD_LENGTH];
}binary_word;

typedef struct data_section {
    size_t DC;
    binary_word **data;
}data_section;

typedef struct instruction_section {
    size_t IC;
    binary_word **instructions;
}instruction_section;

/**
 *@brief Create a .ob (i.e object) file for every filename
 * The .object file is contains the binary representation of the instructions and data
 * The first line in the .ob file consists of 2 fields:
 * 1. The number of instruction words in the object file- stored in IC
 * 2. The number of data words in the object file- stored in DC
 * The following lines contain 2 fields:
 * 1. The address of the word in the memory- in decimal
 * 2. The word itself- in decimal
 * @param inst_section A struct containing the instruction section and the IC
 * @param data__section A struct containing the data section and the DC
 * @param generic_filename A generic filename to create the object file
 * @param num_of_files The number of files to create
 */
status create_object_file(instruction_section inst_section, data_section data__section, char *generic_filename) {
    char *object_filename = NULL;
    FILE *object_file = NULL;
    size_t i;

    object_filename = create_file_name(generic_filename, ".ob");

    object_file = fopen(object_filename, "w");
    if (object_file == NULL) {
        printf("Error: Could not create object file %s\n", object_filename);
        return STATUS_ERROR_WRITE;
    }

    fprintf(object_file, "  Instruction count: %ld Data count: %ld\n", inst_section.IC, data__section.DC);

    for (i = 0; i < inst_section.IC; i++) {
        fprintf(object_file, "%ld %d\n", i, binary_array_to_int(inst_section.instructions[i]->bits));
    }

    for (i = 0; i < data__section.DC; i++) {
        fprintf(object_file, "%ld %d\n", i + inst_section.IC, binary_array_to_int(data__section.data[i]->bits));
    }

    return STATUS_OK;

}