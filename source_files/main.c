#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>
#include "encoding.h"
#include "symbol_table.h"
#include "encode_string.h"
#include "symbol_table.h"
#include "pre_assembler.h"
#include "utilities.h"

int main(int argc, char *argv[])
{

    keyword *keyword_table = NULL;
    macro_table *macroTable = NULL;
    label *label_table = NULL;
    int *decoded_array = NULL;
    char **am_filenames = NULL;

    keyword_table = fill_keywords_table();
    if (keyword_table == NULL) {
        printf("Error while filling keyword table. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    macroTable = fill_macro_table(argc, argv, &am_filenames);
    if (macroTable == NULL) {
        printf("Error while filling macro table. Exiting...\n");
        free(keyword_table);
        exit(EXIT_FAILURE);
    }
    print_macro_table(macroTable);
    label_table = fill_label_table(am_filenames[0], macroTable, keyword_table);
    if (label_table == NULL) {
        printf("Error while filling label table. Exiting...\n");
        delete_filenames(argc - 1, am_filenames);
        macro_table_destructor(macroTable);
        free(keyword_table);
        free(label_table);
        free(decoded_array);
        exit(EXIT_FAILURE);
    }

    print_label_table(label_table);
    decoded_array = decoding(am_filenames[0], label_table, keyword_table);
    if (decoded_array == NULL) {
        printf("Error while decoding. Exiting...\n");
        delete_filenames(argc - 1, am_filenames);
        macro_table_destructor(macroTable);
        free(keyword_table);
        free(label_table);
        free(decoded_array);
        exit(EXIT_FAILURE);
    }
    printf("\nOUTPUT AFTER DECODING LABEL ADDRESS:\n\n");
    print_array_in_binary(decoded_array);

    delete_filenames(argc - 1, am_filenames);
    macro_table_destructor(macroTable);
    free(keyword_table);
    free(label_table);
    free(decoded_array);

    return 0;
}
