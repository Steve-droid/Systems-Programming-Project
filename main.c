#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decode_to_int_array.h"
#include "decode_to_string_array.h"
#include "decoding.h"
#include "label_table.h"
#include "help_table.h"

int main(int argc , char* argv[]){

    keyword* keyword_table;
    macro_table* macroTable;
    label* label_table;
    int* decoded_array;
    char* am_filename;

    keyword_table = fill_keywords_table();
    macroTable = fill_macro_table(argc , argv , &am_filename);
    label_table = fill_label_table(am_filename , macroTable, keyword_table);
    print_label_table(label_table);
    decoded_array = decoding(am_filename , label_table, keyword_table);
    printf("\nOUTPUT AFTER DECODING LABEL ADDRESS:\n\n"); /*temp - no nedded to be here*/
    print_array_in_binary(decoded_array); /*temp - steve continue from here*/

    free(am_filename);
    free(keyword_table);
    free(macroTable);
    free(label_table);
    free(decoded_array);

    return 0;
}