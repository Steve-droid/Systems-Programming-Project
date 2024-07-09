#include "make_label_table.h"

/* Functions */

label* fill_label_table(macro_table* macroTable , keyword* keywords_table){
    char line[MAX_LINE_LENGTH]; /* Buffer to hold each line */
    char label_name[MAX_LABEL_LENGTH]; /* Buffer to hold each label name */
    label* label_table = NULL;
    int labels_counter, table_size, lines_counter;

    initialize_char_array(line);
    initialize_char_array(label_name);

    FILE *file = fopen("mmn14_test.txt", "r"); /* Open the file for reading*/
    if (file == NULL) { /*PROBLEM-FILE NOT EXIST*/
        printf("Error- file doesnt open\n");
        return NULL;
    }

    label_table = create_labels_table();
    table_size = 1;
    labels_counter = 0;
    lines_counter = 0;

    while (fgets(line, sizeof(line), file)) { /* Read every line */
        lines_counter++;
        remove_prefix_spaces(line);
        strncpy(label_name, line, MAX_LABEL_LENGTH);
        if(label_name_is_valid(label_table , label_name, table_size, keywords_table , macroTable)){
            fill_label_data_exclude_address(&label_table[labels_counter] , label_name , lines_counter);
            labels_counter++;
            increase_labels_table(&label_table, table_size + 1);
            table_size++;
        }
    }
    free(&label_table[labels_counter]);
    fill_table_size(label_table, labels_counter);

    fclose(file); // Close the file
    return label_table;
}

label* create_labels_table() {
    return (label*)malloc(sizeof(label));
}

void increase_labels_table(label** table, size_t new_size) {
    *table = (label*)realloc(*table, new_size * sizeof(label));
}

void initialize_char_array(char* char_array){
    int i, array_len;

    array_len = (int)strlen(char_array);

    for( i = 0 ; i < array_len ; i++ ){
        char_array[i] = ' ';
    }
}

int label_name_is_valid(label* label_table, char* label_name, int table_size , keyword* keywords_table , macro_table* macroTable){
    int i;
    char* tmp;

    if(!isalpha(label_name[0])){
        return FALSE;
    }

    for( i = 1 ; label_name[i] != '\0' ; i++ ){
        if(label_name[i] == ':'){
            while( !(tmp = (char*) malloc ( i * sizeof(char) ) ) );
            str_cpy_until_char(tmp,label_name,':');
            if( label_name_is_in_table(label_table,tmp, table_size) ||
                label_name_is_keyword(tmp , keywords_table)         ||
                label_name_is_macro(tmp, macroTable))                          {
                free(tmp);
                return FALSE;
            }
            free(tmp);
            return TRUE;
        }
        if( !( isalpha(label_name[i]) || isdigit(label_name[i]) ) ){
            return FALSE;
        }
    }
    return FALSE;
}

void fill_label_data_exclude_address(label* any_label, char* line, int line_counter){
    static int label_key = FIRST_KEY;

    /* Fill the name */
    str_cpy_until_char((char *) &any_label->name, line , ':');

    /* Fill the key */
    any_label->key = label_key;
    label_key++;

    /* Fill the line where the label was seen */
    any_label->line = line_counter;
}

void remove_prefix_spaces(char* line){
    int i , counter;
    counter = 0;

    while(isspace(line[counter])){
        counter++;
    }

    for(i = counter ; line[i] != '\0' ; i++){
        line[i-counter] = line[i];
    }
    line[i-counter] = '\0';
}

void str_cpy_until_char(char* destination , const char* source , char x){
    int i;

    for( i = 0 ; !(source[i] == '\0' || source[i] == x) ; i++ ){
        destination[i] = source[i];
    }
    destination[i] = '\0';
}

int label_name_is_in_table(label* label_table, char* label_name , int table_size){
    int i;

    for( i = 0 ; i < table_size ; i++ ){
        if( !strncmp(label_table[i].name , label_name , strlen(label_name)) ){
            return TRUE;
        }
    }

    return FALSE;
}

int label_name_is_keyword(char* label_name , keyword* keywords_table){
    int i, label_name_len;

    label_name_len = (int)strlen(label_name);

    for( i = 0 ; i < KEYWORD_TABLE_LENGTH ; i++){
        if(!strncmp(label_name , keywords_table[i].name , label_name_len)){
            return TRUE;
        }
    }

    return FALSE;
}

keyword* fill_keywords_table() {
    keyword *keywords_table;

    while (!(keywords_table = (keyword*)malloc(KEYWORD_TABLE_LENGTH * sizeof(keyword)))) ;

    // Register keywords
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

    // Operation keywords
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

    // Macro keywords
    strcpy(keywords_table[24].name, "macr");
    keywords_table[24].key = MACR;
    keywords_table[24].length = 4;
    strcpy(keywords_table[25].name, "endmacr");
    keywords_table[25].key = ENDMACR;
    keywords_table[25].length = 7;

    // Directive keywords
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

int label_name_is_macro(char* label_name, macro_table* macroTable){
    int i, label_name_len;

    label_name_len = (int)strlen(label_name);

    for( i = 0 ; i < macroTable->size ; i++){
        if(!strncmp(label_name , macroTable[i].name , label_name_len)){
            return TRUE;
        }
    }

    return FALSE;
}

void fill_table_size(label* label_table, int labels_counter){
    int i;

    for(i = 0 ; i < labels_counter ; i++){
        label_table[i].size = labels_counter;
    }
}
