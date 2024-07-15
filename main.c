#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decode_to_int_array.h"
#include "decode_to_string_array.h"
#include "decoding.h"
#include "label_table.h"
#include "help_table.h"
#include "pre_assembler.h"

int main(int argc, char *argv[])
{

    keyword *keyword_table = NULL;
    macro_table *macroTable = NULL;
    label *label_table = NULL;
    int *decoded_array = NULL;
    char *am_filename = NULL;

    keyword_table = fill_keywords_table();
    macroTable = fill_macro_table(argc, argv, &am_filename);
    print_macro_table(macroTable);
    label_table = fill_label_table(am_filename, macroTable, keyword_table);
    print_label_table(label_table);
    decoded_array = decoding(am_filename, label_table, keyword_table);
    printf("\nOUTPUT AFTER DECODING LABEL ADDRESS:\n\n");
    print_array_in_binary(decoded_array);

    free(am_filename);
    free(keyword_table);
    macro_table_destructor(macroTable);
    free(label_table);
    free(decoded_array);

    return 0;
}
