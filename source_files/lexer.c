
#include "lexer.h"


/**
 * @brief Performs the first pass of the assembly process.
 *
 * This function reads an assembly-like file, processes each line to identify commands and arguments,
 * and converts them into a pre-decoded and then a binary format. The results are stored in a 2D array.
 *
 * @param am_filename The name of the assembly file to process.
 * @param _label_table The table containing label information.
 * @param keyword_table The table containing keyword information.
 * @return A 2D array containing the binary representation of each command and its arguments.
 */
 /*Decode without label addresses*/
int **lex(char *am_filename, label_table *_label_table, keyword *keyword_table) {
    char line[MAX_LINE_LENGTH]; /* Buffer to hold each line */
    string *pre_decoded; /*An array that contains the arguments in each cell*/
    int command_name, pre_decoded_size, line_counter, post_decoded_table_size;
    int *post_decoded = NULL;
    int **post_decoded_table = NULL;
    FILE *file = NULL;

    post_decoded_table_size = 0;
    post_decoded_table = (int **)malloc((post_decoded_table_size + 1) * sizeof(int)); /* +1 for NULL terminator*/
    if (post_decoded_table == NULL) {
        printf("ERROR- ALLOCATION FAILED");
        return NULL;
    }
    post_decoded_table[0] = NULL;

    initialize_char_array(line);

    file = fopen(am_filename, "r"); /* Open the file for reading*/
    if (file == NULL) { /*PROBLEM-FILE NOT EXIST*/
        printf("Error- CAN'T OPEN FILE\n");
        free(post_decoded_table);
        return NULL;
    }

    command_name = pre_decoded_size = 0;
    line_counter = -1; /*first line is 0*/

    printf("PRE-DECODED:\n");

    while (fgets(line, sizeof(line), file)) { /* Read every line */
        if (is_empty_line(line)) {
            continue;
        }

        remove_prefix_spaces(line);
        if (line[0] == ';') {
            continue;
        }

        line_counter++;
        command_name = identify_command(line, _label_table, keyword_table, line_counter); /*decipher the command name*/
        pre_decoded_size = get_command_argument_count(command_name); /*Returns the amount of cells that needed for every argument*/

        if (command_name == UNDEFINED || pre_decoded_size == UNDEFINED) { /*undefined command name*/
            printf("ERROR- undefined command name");
            free(post_decoded_table);
            return NULL;
        }

        /*creates the right size that needed to save the data*/
        if (pre_decoded_size == UNKNOWN_NUMBER_OF_ARGUMENTS) { /*the array that contains unknown arguments */
            pre_decoded = (string *)malloc(2 * sizeof(string)); /*array[0]: command name , array[1]: command data*/
            if (pre_decoded == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                free(post_decoded_table);
                return NULL;
            }
        }
        else {
            pre_decoded = (string *)malloc(pre_decoded_size * sizeof(string)); /* every cell contains a word*/
            if (pre_decoded == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                free(post_decoded_table);
                return NULL;
            }
        }

        if (!fill_pre_decoded_array(line, pre_decoded, pre_decoded_size, keyword_table, command_name, _label_table, line_counter)) { /*Fill the pre-decoded array*/
            return NULL;
        }
        print_pre_decoded(pre_decoded, pre_decoded_size); /*tmp- to see predocded*/

        post_decoded = convert_pre_lexed_fields_to_binary(pre_decoded, pre_decoded_size, keyword_table, command_name, _label_table); /*the array after decoding in decimal integers*/
        if (post_decoded != NULL) {
            post_decoded_table_size++;

            post_decoded_table = (int **)realloc(post_decoded_table, (post_decoded_table_size + 1) * sizeof(int *)); /*1 for flag-NULL*/
            if (post_decoded_table == NULL) {
                printf("ERROR- ALLOCATION FAILED");
                free(post_decoded_table);
                return NULL;
            }

            post_decoded_table[post_decoded_table_size - 1] = post_decoded;
            post_decoded_table[post_decoded_table_size] = NULL;
        }
    }

    printf("line counter is: %d\n", line_counter);
    free(pre_decoded);
    fclose(file); /* Close the file */

    print_2D_array(post_decoded_table);

    return post_decoded_table;
}

