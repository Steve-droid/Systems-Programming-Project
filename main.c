#include "make_label_table.h"
#include "decoding.h"

macro_table* create_test_macro_table();
void print_label_table(label* label_table);
void print_array_in_binary(int* array, int size);
void print_binary(int num);

int main(){
    keyword* keyword_table;
    macro_table* macroTable;
    label* label_table;
    int* decimal_decoded_array;
    int decimal_decoded_array_size;

    printf("OUTPUT: \n\n"); /*temp - start of output*/

    keyword_table = fill_keywords_table();
    macroTable = create_test_macro_table(); /*temp- instead of real macro-table*/
    label_table = fill_label_table(macroTable, keyword_table);
    print_label_table(label_table); /*temp - no nedded to be here*/
    decimal_decoded_array = decoding(label_table, keyword_table, &decimal_decoded_array_size);

    free(keyword_table);
    free(macroTable);
    free(label_table);

    print_array_in_binary(decimal_decoded_array, decimal_decoded_array_size);

    free(decimal_decoded_array);

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

void print_label_table(label* label_table){
    int i;
    if(label_table == NULL){
        printf("Error NULL FILE");
        return;
    }

    for(i = 0 ; i < label_table[0].size ; i++ ){
        printf("name: %s \n",label_table[i].name);
        printf("key: %d \n",label_table[i].key);
        printf("line: %d \n",label_table[i].line);
        printf("size: %d \n\n",label_table[i].size);

    }
}

void print_array_in_binary(int* array, int size){
    int i;

    for( i = 0 ; i < size ; i++ ){
        print_binary(array[i]);
    }
}

void print_binary(int num) {
    /* Allocate a string to hold the binary representation (16 chars: 15 bits + null terminator) */
    char binary[16];
    binary[15] = '\0';  /* Null terminator */

    /* Iterate through each bit, starting from the MSB */
    for (int i = 14; i >= 0; i--) {
        binary[i] = (num & 1) ? '1' : '0';
        num >>= 1;  /* Shift the number to the right */
    }

    /* Print the resulting binary string */
    printf("%s\n", binary);
}
