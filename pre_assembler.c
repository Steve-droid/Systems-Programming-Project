#include "pre_assembler.h"

void compute_as_file_name(char *src_file_name, char *as_file_name) {

    unsigned int src_file_name_length = strlen(src_file_name);
    unsigned int as_extention_length = strlen(as_extention);

    as_file_name = (char *)malloc(src_file_name_length + as_extention_length + 1);
    if (as_file_name == NULL) {
        err(errno, "Failed to allocate memory for as file name");
    }

    strcpy(as_file_name, src_file_name);
    strcat(as_file_name, as_extention);

}

void compute_am_file_name(char *src_file_name, char *am_file_name) {

    unsigned int src_file_name_length = strlen(src_file_name);
    unsigned int am_extention_length = strlen(am_extention);

    am_file_name = (char *)malloc(src_file_name_length + am_extention_length + 1);
    if (am_file_name == NULL) {
        err(errno, "Failed to allocate memory for am file name");
    }

    strcpy(am_file_name, src_file_name);
    strcat(am_file_name, am_extention);

}

void parse_line(char *line, macro_state *state) {

    /* Check if the line is empty */
    if (line[0] == '\n') {
        *state = empty_line;
        return;
    }

    /* Check if the line is a macro definition */
    if (line[0] == '#') {
        *state = definition;
        return;
    }

    /* Check if the line is a macro call */
    if (line[0] == '.') {
        *state = call;
        return;
    }

    /* Check if the line is a macro end */
    if (line[0] == '!') {
        *state = illegeal_end_macro;
        return;
    }

    /* Check if the line is a macro redefinition */
    if (line[0] == '$') {
        *state = macro_redefinition_error;
        return;
    }

    /* Check if the line is an illegal macro definition */
    if (line[0] == '@') {
        *state = illegal_definition;
        return;
    }

    /* Check if the line is an illegal macro call */
    if (line[0] == '*') {
        *state = illegal_call;
        return;
    }

    /* If the line is not empty and not a macro definition, call or end, it is a regular line */
    *state = line_other;

}




void pre_assemble(FILE *am_file, char *am_file_name) {

    /* A buffer for reading lines from the .as file inside a macro definition */
    char line_buffer[MAX_LINE_LENGTH] = { 0 };

    /* Define variables for Macro Table */
    vector *macro_table;
    macro *current_macro = NULL;

    /* Define and initiallize macro state */
    macro_state state = empty_line;
    int line_count = 0;


    /* Read the .am file line by line */
    while (fgets(line_buffer, MAX_LINE_LENGTH, am_file) != NULL) {

        /* Increment the line count */
        line_count++;

        /* Check for EOF */
        if (feof(am_file)) {
            printf("EOF\n");
            return;
        }

        /* Check the state of the line */
        switch (state) {
        case definition:
            if (current_macro == NULL) {
                current_macro = macro_constructor(line_buffer);
            }
            else {
                vector_insert(current_macro->lines, strdup(line_buffer));
                current_macro->line_count++;
            }
            break;
        case empty_line:
            if (current_macro != NULL) {
                insert_macro(macro_table, current_macro);
                current_macro = NULL;
            }
            break;
        case line_other:
            if (current_macro != NULL) {
                vector_insert(current_macro->lines, strdup(line_buffer));
                current_macro->line_count++;
            }
            break;
        case macro_redefinition_error:
            errx(1, "Error: Macro redefinition error in line %d", line_count);
            break;
        case illegal_definition:
            errx(1, "Error: Illegal macro definition in line %d", line_count);
            break;
        case illegeal_end_macro:
            errx(1, "Error: Illegal end macro in line %d", line_count);
            break;
        case call:
            break;
        case illegal_call:
            break;
        default:
            break;
        }
    }



}

