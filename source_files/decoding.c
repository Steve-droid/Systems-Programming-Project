#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decoding.h"
#include "decode_to_int_array.h"
#include "decode_to_string_array.h"
#include "label_table.h"
#include "utilities.h"


/* The function makes a binary code*/

int *decoding(char *am_filename, label *label_table, keyword *keyword_table) {
    int *decoded = NULL;
    int **decoded_table = NULL;

    decoded_table = decode_without_label_addresses(am_filename, label_table, keyword_table);
    decoded = convert_to_1D(decoded_table);
    printf("\nPOST-DECODED (LABEL PRESENTED AS ITS KEY):\n");
    print_array_in_binary(decoded);



    fill_label_table_addresses(decoded_table, label_table);
    decoded = convert_to_1D(decoded_table);
    decode_label_addersses(decoded, label_table);

    free(decoded_table);
    return decoded;
}

void fill_label_table_addresses(int** decoded_table , label* label_table){
    int i,j,k,counter;

    for( k = 0 ; k < label_table[0].size ; k++){
        counter = FIRST_ADDRESS;
        for( i = 0 ; decoded_table[i] != NULL && i < label_table[k].line ; i++ ){
            for(j = 0 ; decoded_table[i][j] != FLAG ; j++){
                counter++;
            }
        }
        label_table[k].address = counter;
        label_table[k].address = label_table[k].address << 3;
        label_table[k].address += label_table[k].entry_or_extern == EXTERN ? 1 : 2 ;
    }
}

void decode_label_addersses(int* decoded , label* label_table){
    int i,k;

    for( k = 0 ; k < label_table[0].size ; k++){
        for( i = 0 ; decoded[i] != FLAG ; i++ ){
            if( label_table[k].key == decoded[i]){
                decoded[i] = label_table[k].address;
            }
        }
    }
}
