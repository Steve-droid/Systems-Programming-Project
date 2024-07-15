#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>
#include "decode_to_int_array.h"
#include "decode_to_string_array.h"
#include "decoding.h"
#include "label_table.h"
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
    macroTable = fill_macro_table(argc, argv, &am_filenames);
    print_macro_table(macroTable);
    label_table = fill_label_table(am_filenames[0], macroTable, keyword_table);
    print_label_table(label_table);
    decoded_array = decoding(am_filenames[0], label_table, keyword_table);
    printf("\nOUTPUT AFTER DECODING LABEL ADDRESS:\n\n");
    print_array_in_binary(decoded_array);

    delete_filenames(argc - 1, am_filenames);
    macro_table_destructor(macroTable);
    free(keyword_table);
    free(label_table);
    free(decoded_array);

    return 0;
