#include "decoding.h"

#define UNUSED(x) (void)(x)


/* The function makes a binary code*/
int* decoding(label* label_table, keyword* keyword_table, int* decoded_array_size){
    UNUSED(decoded_array_size);

    printf("ENTERED DECODING FUNC\n");

    char line[MAX_LINE_LENGTH]; /* Buffer to hold each line */
    string* pre_decoded; /*An array that contains the arguments in each cell*/
    int command_name , pre_decoded_size, line_counter, post_decoded_size;
    int* post_decoded;

    initialize_char_array(line);

    FILE *file = fopen("mmn14_test.txt", "r"); /* Open the file for reading*/
    if (file == NULL) { /*PROBLEM-FILE NOT EXIST*/
        printf("Error- file doesnt open\n");
        return NULL;
    }

    command_name = line_counter = pre_decoded_size = post_decoded_size = 0;

    while (fgets(line, sizeof(line), file)) { /* Read every line */
        line_counter++;
        remove_prefix_spaces(line); 
        command_name = deciphering_command(line ,label_table, keyword_table, line_counter); /*decipher the command name*/
        pre_decoded_size = arguments_amount_for_command(command_name); /*Returns the amount of cells that needed for every argument*/

        if(command_name == UNDEFINED || pre_decoded_size == UNDEFINED){ /*undefined command name*/
            printf("ERROR- undefined command name");
            return NULL;
        }

        /*creates the right size that needed to save the data*/
        if(pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS){ /*the array that contains unknown arguments */
           while(!( pre_decoded = (string*)malloc(2 * sizeof(string)) ) ); /*array[0]: command name , array[1]: command data*/
        }
        else{
            while(!(pre_decoded = (string*)malloc(pre_decoded_size * sizeof(string)) ) ); /* every cell contains a word*/
        }

        fill_pre_decoded(line, pre_decoded, pre_decoded_size, keyword_table, command_name, label_table, line_counter); /*Fill the pre-decoded array*/
        post_decoded = decode_pre_decoded(pre_decoded , &post_decoded_size); /*the array after decoding in decimal integers*/
        free(pre_decoded);

        fclose(file); // Close the file
    }
    return post_decoded;
}

int arguments_amount_for_command(int command_name){
        switch(command_name){
            case MOV:
            case CMP:
            case ADD:
            case SUB:
            case LEA:
                return 3;
            break;

            case CLR:
            case NOT:
            case INC:
            case DEC:
            case JMP:
            case BNE:
            case RED:
            case PRN:
            case JSR:
                return 2;

            case RTS:
            case STOP:
                return 1;

            case DATA:
            case STRING:
            case ENTRY:
            case EXTERN:
                return UNKNOWN_NUMBER_OF_ARGUMENTS;

            default:
                return UNDEFINED;
        }
}

int deciphering_command(char* line, label* label_table, keyword* keyword_table, int current_line){
    int i;
    char* line_from_first_letter;
    char commend_name[MAX_KEYWORD_LENGTH];

    line_from_first_letter = pointer_after_label(line, label_table, current_line);

    for(i = 0 ; line_from_first_letter[i] != '\0' && !isspace(line_from_first_letter[i]) ; i++){
        commend_name[i] = line_from_first_letter[i];
    }

    for(i = 0 ; i < KEYWORD_TABLE_LENGTH ; i++){
        if(! strncmp(commend_name,keyword_table[i].name,strlen(keyword_table[i].name))){
            return keyword_table[i].key;
        }
    }
    return UNDEFINED;
}

void fill_pre_decoded(char* line, string* pre_decoded, int pre_decoded_size, keyword* keyword_table, int command_name_key, label* label_table, int current_line){
    int command_number;
    char* line_from_first_letter;

    /*adding the first argument- the command (in array[0])*/
    command_number = command_number_by_key(keyword_table, command_name_key);
    strcpy(pre_decoded[0].data,keyword_table[command_number].name);

    /*update the line pointer from the rellevant cell*/
    line_from_first_letter = pointer_after_label(line, label_table, current_line); /*skip the label (if it there) and spaces*/
    line_from_first_letter = &line_from_first_letter[keyword_table[command_number].length]; /*skip the command*/
    remove_prefix_spaces(line_from_first_letter); /*skip unnecessary spaces*/

    if(line_from_first_letter[0] == ','){ /*unnecessary comma between command and arguments*/
        printf("ERROR- An unnecessary comma between command and arguments\n");
        return;
    }

    if(pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS){ /* keep all the arguments in array[1] */
        fill_pre_decoded_unknown_arguments_amount(line_from_first_letter, command_number, keyword_table, pre_decoded);
    }
    else{
        fill_pre_decoded_known_arguments_amount();
    }


}

char* pointer_after_label(char* line, label* label_table, int current_line){
    int i, first_letter;
    char* line_from_first_letter;

    first_letter = 0;

    /*find the first letter after the label name*/
    for(i = 0 ; i < label_table[0].size ; i++){
        if(label_table[i].line == current_line){
            first_letter = strlen(label_table[i].name);
        }
    }

    /*point on the new "array"*/
    line_from_first_letter = &line[first_letter];
    /*remove unnecessary spaces*/
    remove_prefix_spaces(line_from_first_letter);

    return line_from_first_letter;
}

void fill_pre_decoded_unknown_arguments_amount(char* line, int command_number, keyword* keyword_table, string* pre_decoded){
    int i, j, comma, end_of_argument, end_of_string;

    comma = end_of_argument = end_of_string = FALSE;

    for(i = 0 , j = 0 ; line[i] != '\0' && line[i] != '\n' ; i++){
        if(strcmp(keyword_table[command_number].name , ".data")){ /* in case of .data */
            if(line[i] == ','){
                if(comma == TRUE){
                        printf("ERROR- A series of commas without a number between them\n");
                        return;
                }
                comma = end_of_argument = TRUE;
                pre_decoded[1].data[j++] = ',';
            }
            else if(isspace(line[i])){
                end_of_argument = TRUE;
            }
            if( (line[i] >= '0' && line[i] <= '9') ||
                line[i] == '-' || line[i] == '+'){
                    if(end_of_argument == TRUE && comma == FALSE){ /* 2 integers without comma between them*/
                        printf("ERROR- NO COMMA BETWEEN INTEGERS\n");
                        return;
                    }
                    comma = end_of_argument = FALSE;
                    pre_decoded[1].data[j++] = line[i];
            }
            else{ /* symbol that doesnt need to be in "".data" data */
                printf("ERROR- Invalid character in content of data\n");
                    return;
            }

        }
        else if(strcmp(keyword_table[command_number].name , ".string")){  /* in case of ".string" */
            if( i == 0 ){
                if(line[0] != '\"'){ /* The first data of ".string" is not " */
                    printf("ERROR- There is no \" at the beginning of the data in .string command\n"); 
                    return;
                }
                else{
                    i++;
                }
            }
            if(( !isspace(line[i]) ) && end_of_argument == TRUE){ /*in case of more than 1 argument*/
                if(line[i] == '\"'){
                    printf("ERROR- Too many \"\n");
                    return;
                }
                printf("ERROR- There are too many arguments\n");
                return;
            }

            /*copy the string data into array 1*/
            if(isspace(line[i] || line[i] == '\"')){
                end_of_argument = TRUE;
                if(line[i] == '\"'){
                    end_of_string = TRUE;
                }
            }
            else{
                if(!end_of_string){
                    if(isalpha(line[i]) || isdigit(line[i])){
                        pre_decoded[1].data[j++] = line[i];
                    }
                    else{
                        printf("ERROR- Invalid character in argument, no such label exists\n");
                        return;
                    }
                }
            }
                
            if( ( line[i+1] == '\0' || line[i+1] == '\n' ) && line[i] != '\"' && !end_of_string){ /* The string doesnt have the ending " */
                printf("ERROR- There is no \" at the end of the data in .string command\n"); 
                return;                    
            }
        }
        else{ /* ".entry" , ".extern" */
            if( ! ( isalpha(line[i]) || isdigit(line[i]))){
                printf("ERROR- Invalid character for label, no label with this name");
                return;
            }
            if( (!isspace(line[i])) && end_of_argument == TRUE){
                printf("ERROR- Too many arguments");
                return;
            }
            if(isspace(line[i]) ||  line[i+1] == '\0' || line[i+1] == '\n' ){
                end_of_argument = TRUE;
            }
            if(!isspace(line[i])){
                pre_decoded[1].data[j++] = line[i];
            }
        }
    }
}

void fill_pre_decoded_known_arguments_amount(){

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

int* decode_pre_decoded(string* pre_decoded , int* post_decoded_size){
    UNUSED(pre_decoded);
    UNUSED(post_decoded_size);
    return NULL;
}