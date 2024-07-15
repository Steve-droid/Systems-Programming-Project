#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decode_to_int_array.h"
#include "decoding.h"
#include "label_table.h"
#include "decode_to_string_array.h"
#include "help_table.h"


void initialize_char_array(char* char_array){
    int i, array_len;

    array_len = (int)strlen(char_array);

    for( i = 0 ; i < array_len ; i++ ){
        char_array[i] = ' ';
    }
}

void remove_prefix_spaces(char* line) {
    int i, counter;
    counter = 0;

    /** Count leading spaces */
    while (line[counter] != '\0' && isspace((unsigned char)line[counter])) {
        counter++;
    }

    /** Shift the rest of the line to the front */
    if (counter > 0) {
        for (i = 0; line[i + counter] != '\0'; i++) {
            line[i] = line[i + counter];
        }
        line[i] = '\0';  /** Null-terminate the shifted string */
    }
}

char* pointer_after_label(char* line, label* label_table, int current_line) {
    int i, first_letter = 0;

    /** Find the first letter after the label name */
    for (i = 0; i < label_table[0].size; i++) {
        if (label_table[i].line == current_line) {
            first_letter = 1 + strlen(label_table[i].name);  /** another 1 for ':' */
            break;  /** Exit loop once the label is found */
        }
    }

    /** Point to the new "array" */
    return line + first_letter;
}

int command_number_by_key(keyword* keyword_table, int key){
    int i , flag;

    flag = UNDEFINED;

    for(i = 0 ; i < KEYWORD_TABLE_LENGTH ; i++){
        if(key == keyword_table[i].key){
            flag = i;
        }
    }
    return flag;
}

int map_addressing_method(char* str, label* label_table){
    int i;
    
    /* case 0 */
    if(str[0] == '#'){
        if(str[1] == '\0'){
            return UNDEFINED;
        }
        if( str[1] != '-' && str[1] != '+' && !isdigit(str[1]) ){
            return UNDEFINED;
        }
        if( str[1] == '-' || str[1] == '+'){
            if( !isdigit(str[2]) ){
                return UNDEFINED;
            }
        }
        for(i = 2 ; i < (int)strlen(str) ; i++){
            if( !isdigit(str[i]) ){
                return UNDEFINED;
            }
        }
        return 0;
    }

    /* case 1 */
    for(i = 0 ; i < label_table[0].size ; i++){
        if(!strcmp(label_table[i].name,str)){
            return 1;
        }
    }

    /* case 2 */
    if(str[0] == '*'){
        if(str[1] == '\0' || str[1] != 'r'){
            return UNDEFINED;
        }
        if(str[2] == '\0' ||  str[2] < '0' || str[2] > '7'){
            return UNDEFINED;
        }
        if(str[3] != '\0'){
            return UNDEFINED;
        }
        return 2;
    }

    /* case 3 */
    
    if(str[0] == 'r'){
        if(str[1] == '\0' || str[1] < '0' || str[1] > '7'){
            return UNDEFINED;
        }
        if(str[2] != '\0'){
            return UNDEFINED;
        }
        return 3;
    }

    /* else */
    return UNDEFINED;
}

void print_pre_decoded(string* pre_decoded, int pre_decoded_size){
    int i;

    if(pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS){
        pre_decoded_size = 2;
    }

    for( i = 0 ; i < pre_decoded_size ; i++){
        if(i != pre_decoded_size-1){
            printf("%s___",pre_decoded[i].data );
        }
        else{
            printf("%s\n",pre_decoded[i].data );
        }
    }
}

void int_to_binary_array(int num, int *arr, int from_cell, int to_cell) {
    int i;
    /** If the number is negative, adjust for two's complement */
    if (num < 0) {
        num += (1 << (to_cell - from_cell + 1));
    }

    /** Convert the number to binary and store it in the array */
    for ( i = to_cell; i >= from_cell; i--) {
        arr[i] = num % 2;
        num /= 2;
    }
}

int binary_array_to_int(int *array) {
    int sign_bit,value,i;
    sign_bit = array[0];
    value = 0;

    /** Iterate over the array to construct the integer value */
    for (i = 0; i < OUTPUT_COMMAND_LEN; i++) {
        value = (value << 1) | array[i];
    }

    /** If the sign bit is 1, the number is negative in two's complement */
    if (sign_bit == 1) {
        /** Subtract 2^15 to convert to negative number */
        value -= (1 << OUTPUT_COMMAND_LEN);
    }

    return value;
}

int* convert_to_1D(int** array2D) {
    int total_elements,i ,j, index;
    int* array1D = NULL;

    
    if(array2D == NULL){
        return NULL;
    }
    /* Calculate the total number of elements (excluding FLAGs) */
    total_elements = 0;
    for (i = 0; array2D[i] != NULL; i++) {
        for (j = 0; array2D[i][j] != FLAG; j++) {
            total_elements++;
        }
    }

    /* Allocate memory for the 1D array plus one additional space for the FLAG */
    array1D = (int*)malloc((total_elements + 1) * sizeof(int));
    if (array1D == NULL) {
        printf( "ERROR-Memory allocation failed\n");
        return NULL;
    }

    /* Copy elements from the 2D array to the 1D array */
    index = 0;
    for ( i = 0; array2D[i] != NULL; i++) {
        for (j = 0; array2D[i][j] != FLAG; j++) {
            array1D[index++] = array2D[i][j];
        }
    }

    /* Add the FLAG as the terminator of the 1D array */
    array1D[index] = FLAG;

    return array1D;
}

void initialize_int_array(int* arr , int size){
    int i;
    for(i = 0 ; i < size ; i++){
        arr[i] = 0;
    }
}

void print_array_in_binary(int* arr){
    int i;
    if(arr == NULL){
        return;
    }

    for( i = 0 ; arr[i] != FLAG ; i++ ){
        print_binary(arr[i]);
    }
}

void print_binary(int num) {
    int i;
    char binary[16];
    binary[15] = '\0';  /* Null terminator */

    unsigned int mask = 1 << 14;  /* Mask for the 15th bit */

    /** If the number is negative, convert it to 2's complement representation */
    unsigned int u_num;
    if (num < 0) {
        u_num = (unsigned int)(num + (1 << 15));  /* 2's complement for negative number */
    } else {
        u_num = (unsigned int)num;
    }

    /** Check if the number is greater than or equal to FIRST_KEY and print it if so */
    if (num >= FIRST_KEY) {
        printf("%d\n", num);
        return;
    }

    /** Create the binary string */
    for (i = 0; i < 15; i++) {
        binary[i] = (u_num & mask) ? '1' : '0';
        mask >>= 1;
    }

    /** Print the resulting binary string */
    printf("%s\n", binary);
}

void print_label_table(label* label_table){
    int i;

    printf("LABEL-TABLE:\n");
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

keyword* fill_keywords_table() {
    keyword *keywords_table = NULL;

    keywords_table = (keyword*)malloc(KEYWORD_TABLE_LENGTH * sizeof(keyword)) ;
    if(keywords_table == NULL){
        printf("ERROR-ALLOCATION FAILED\n");
        return NULL;
    }

    /* Register keywords */
    strcpy(keywords_table[0].name, "r0");
    keywords_table[0].key = R0;
    keywords_table[0].length = 2;
    strcpy(keywords_table[1].name, "r1");
    keywords_table[1].key = R1;
    keywords_table[1].length = 2;
    strcpy(keywords_table[2].name, "r2");
    keywords_table[2].key = R2;
    keywords_table[2].length = 2;
    strcpy(keywords_table[3].name, "r3");
    keywords_table[3].key = R3;
    keywords_table[3].length = 2;
    strcpy(keywords_table[4].name, "r4");
    keywords_table[4].key = R4;
    keywords_table[4].length = 2;
    strcpy(keywords_table[5].name, "r5");
    keywords_table[5].key = R5;
    keywords_table[5].length = 2;
    strcpy(keywords_table[6].name, "r6");
    keywords_table[6].key = R6;
    keywords_table[6].length = 2;
    strcpy(keywords_table[7].name, "r7");
    keywords_table[7].key = R7;
    keywords_table[7].length = 2;

    /* Operation keywords */
    strcpy(keywords_table[8].name, "mov");
    keywords_table[8].key = MOV;
    keywords_table[8].length = 3;
    strcpy(keywords_table[9].name, "cmp");
    keywords_table[9].key = CMP;
    keywords_table[9].length = 3;
    strcpy(keywords_table[10].name, "add");
    keywords_table[10].key = ADD;
    keywords_table[10].length = 3;
    strcpy(keywords_table[11].name, "sub");
    keywords_table[11].key = SUB;
    keywords_table[11].length = 3;
    strcpy(keywords_table[12].name, "lea");
    keywords_table[12].key = LEA;
    keywords_table[12].length = 3;
    strcpy(keywords_table[13].name, "clr");
    keywords_table[13].key = CLR;
    keywords_table[13].length = 3;
    strcpy(keywords_table[14].name, "not");
    keywords_table[14].key = NOT;
    keywords_table[14].length = 3;
    strcpy(keywords_table[15].name, "inc");
    keywords_table[15].key = INC;
    keywords_table[15].length = 3;
    strcpy(keywords_table[16].name, "dec");
    keywords_table[16].key = DEC;
    keywords_table[16].length = 3;
    strcpy(keywords_table[17].name, "jmp");
    keywords_table[17].key = JMP;
    keywords_table[17].length = 3;
    strcpy(keywords_table[18].name, "bne");
    keywords_table[18].key = BNE;
    keywords_table[18].length = 3;
    strcpy(keywords_table[19].name, "red");
    keywords_table[19].key = RED;
    keywords_table[19].length = 3;
    strcpy(keywords_table[20].name, "prn");
    keywords_table[20].key = PRN;
    keywords_table[20].length = 3;
    strcpy(keywords_table[21].name, "jsr");
    keywords_table[21].key = JSR;
    keywords_table[21].length = 3;
    strcpy(keywords_table[22].name, "rts");
    keywords_table[22].key = RTS;
    keywords_table[22].length = 3;
    strcpy(keywords_table[23].name, "stop");
    keywords_table[23].key = STOP;
    keywords_table[23].length = 4;

    /* Macro keywords */
    strcpy(keywords_table[24].name, "macr");
    keywords_table[24].key = MACR;
    keywords_table[24].length = 4;
    strcpy(keywords_table[25].name, "endmacr");
    keywords_table[25].key = ENDMACR;
    keywords_table[25].length = 7;

    /* Directive keywords */
    strcpy(keywords_table[26].name, ".data");
    keywords_table[26].key = DATA;
    keywords_table[26].length = 5;
    strcpy(keywords_table[27].name, ".string");
    keywords_table[27].key = STRING;
    keywords_table[27].length = 7;
    strcpy(keywords_table[28].name, ".entry");
    keywords_table[28].key = ENTRY;
    keywords_table[28].length = 6;
    strcpy(keywords_table[29].name, ".extern");
    keywords_table[29].key = EXTERN;
    keywords_table[29].length = 7;

    return keywords_table;
}

int is_empty_line(char* str) {
    /* Remove the newline character at the end of the line, if there is one */
    str[strcspn(str, "\n")] = '\0';

    /* Check if the line is empty */
    if(strlen(str) == 0) {
        return TRUE; /* Line is empty */
    }

    return FALSE; /* Line is not empty */
}

int char_to_int(char c) {
    if (isdigit(c)) {
        return c - '0';
    } else if( c == '+' ){
        return PLUS;
    } else if( c == '-'){
        return MINUS;
    } else if( c == ','){
        return  COMMA;
    }
    else{
        printf("Error: The character is not a digit.\n");
        return -1; /* Return an error code if the character is not a digit */
    }
}

void remove_commas_from_str(char* str) {
    int i, j;

    for (i = 0, j = 0; str[i] != '\0' && str[i] != '\n'; i++) {
        if (str[i] != ',') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void shift_left_str(char* str, int steps) {
    /* Get the length of the string */
    int length = strlen(str);
    
    /* If steps are greater than or equal to string length, the result is an empty string */
    if (steps >= length) {
        str[0] = '\0';
        return;
    }

    /* Loop to shift the characters to the left by 'steps' positions */
    int i;
    for (i = 0; i + steps < length; i++) {
        str[i] = str[i + steps];
    }
    
    /* Properly terminate the string */
    str[i] = '\0';
}

int* convert_to_int_array(char* str) {
    /* Temporary variables */
    int* result = NULL;
    char* token = NULL;
    int index = 0;

    /* Tokenize the string and convert each token to an integer */
    token = strtok(str, ",");
    while (token != NULL) {
        /* Reallocate memory for the integer array */
        result = (int*)realloc(result, (index + 1) * sizeof(int));
        if (result == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        /* Convert token to integer and store in the array */
        result[index++] = atoi(token);
        
        /* Get next token */
        token = strtok(NULL, ",");
    }

    /* Reallocate memory to add the FLAG at the end */
    result = (int*)realloc(result, (index + 1) * sizeof(int));
    if (result == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    /* Set the FLAG at the end of the array */
    result[index] = FLAG;

    return result;
}

void print_2D_array(int** arr){
    int i,j,counter;
    counter = 99;

    printf("THE 2D ARRAY:\n");
    for(i=0 ; arr[i] != NULL ; i++){
        for(j = 0 ; arr[i][j] != FLAG ; j++){
            counter++;
            printf("line: %d, number: %d\n",counter,arr[i][j]);
        }
    }
    printf("END 2D ARRAY\n");
}
