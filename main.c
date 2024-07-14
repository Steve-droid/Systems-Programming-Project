#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decode_to_int_array.h"
#include "decode_to_string_array.h"
#include "decoding.h"
#include "label_table.h"
#include "help_table.h"

macro_table* create_test_macro_table();

int main(){
    keyword* keyword_table;
    macro_table* macroTable;
    label* label_table;
    int* decoded_array;

    keyword_table = fill_keywords_table();
    macroTable = create_test_macro_table(); /*temp- instead of real macro-table*/
    label_table = fill_label_table(macroTable, keyword_table);
    print_label_table(label_table); /*temp - no nedded to be here*/
    decoded_array = decoding(label_table, keyword_table);
    printf("\nOUTPUT AFTER DECODING LABEL ADDRESS:\n\n");
    print_array_in_binary(decoded_array);

    free(keyword_table);
    free(macroTable);
    free(label_table);
    free(decoded_array);

    return 0;
}

macro_table* create_test_macro_table(){
    int i;
    macro_table* table = (macro_table*)malloc(5 * sizeof (macro_table));
    for(i = 0 ; i < 5 ; i++){
        table[i].size = 5;
    }
    strcpy(table[0].name,"a");
    strcpy(table[1].name,"b");
    strcpy(table[2].name,"c");
    strcpy(table[3].name,"d");
    strcpy(table[4].name,"e");

    return table;
}
