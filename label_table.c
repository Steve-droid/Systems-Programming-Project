#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "label_table.h"
#include "decoding.h"
#include "decode_to_int_array.h"
#include "decode_to_string_array.h"
#include "macro.h"


/* Functions */

label *fill_label_table(char *am_filename, macro_table *macroTable, keyword *keywords_table) {
    char line[MAX_LINE_LENGTH]; /* Buffer to hold each line */
    char label_name[MAX_LABEL_LENGTH]; /* Buffer to hold each label name */
    label *label_table = NULL;
    int labels_counter, table_size, lines_counter;
    FILE *file = NULL;

    initialize_char_array(line);
    initialize_char_array(label_name);

    file = fopen(am_filename, "r"); /* Open the file for reading*/
    if (file == NULL) { /*PROBLEM-FILE NOT EXIST*/
        printf("Error- file doesnt open\n");
        return NULL;
    }

    printf("%s\n", am_filename);

    label_table = create_labels_table();
    table_size = 1;
    labels_counter = 0;
    lines_counter = -1; /* lines start from 0*/

    while (fgets(line, sizeof(line), file)) { /* Read every line */
        if (is_empty_line(line)) {
            continue;
        }

        remove_prefix_spaces(line);
        if (line[0] == ';') {
            continue;
        }

        lines_counter++;
        remove_prefix_spaces(line);
        strncpy(label_name, line, MAX_LABEL_LENGTH);
        if (label_name_is_valid(label_table, label_name, table_size, keywords_table, macroTable)) {
            fill_label_data_exclude_address(&label_table[labels_counter], label_name, lines_counter);
            labels_counter++;
            increase_labels_table(&label_table, table_size + 1);
            table_size++;
        }
    }
    /* free(&label_table[labels_counter]); */
    fill_table_size(label_table, labels_counter);

    fclose(file); /* Close the file */
    return label_table;
}

label *create_labels_table() {
    return (label *)malloc(sizeof(label));
}

void increase_labels_table(label **table, size_t new_size) {
    *table = (label *)realloc(*table, new_size * sizeof(label));
}

int label_name_is_valid(label *label_table, char *label_name, int table_size, keyword *keywords_table, macro_table *macroTable) {
    int i;
    char *tmp;

    if (!isalpha(label_name[0])) {
        return FALSE;
    }

    for (i = 1; label_name[i] != '\0'; i++) {
        if (label_name[i] == ':') {
            while (!(tmp = (char *)malloc(i * sizeof(char))));
            str_cpy_until_char(tmp, label_name, ':');
            if (label_name_is_in_table(label_table, tmp, table_size) ||
                label_name_is_keyword(tmp, keywords_table) ||
                label_name_is_macro(tmp, macroTable)) {
                free(tmp);
                return FALSE;
            }
            free(tmp);
            return TRUE;
        }
        if (!(isalpha(label_name[i]) || isdigit(label_name[i]))) {
            return FALSE;
        }
    }
    return FALSE;
}

void fill_label_data_exclude_address(label *any_label, char *line, int line_counter) {
    static int label_key = FIRST_KEY;

    /* Fill the name */
    str_cpy_until_char((char *)&any_label->name, line, ':');

    /* Fill the key */
    any_label->key = label_key;
    label_key++;

    /* Fill the line where the label was seen */
    any_label->line = line_counter;
}

void str_cpy_until_char(char *destination, const char *source, char x) {
    int i;

    for (i = 0; !(source[i] == '\0' || source[i] == x); i++) {
        destination[i] = source[i];
    }
    destination[i] = '\0';
}

int label_name_is_in_table(label *label_table, char *label_name, int table_size) {
    int i;

    for (i = 0; i < table_size; i++) {
        if (!strncmp(label_table[i].name, label_name, strlen(label_name))) {
            return TRUE;
        }
    }

    return FALSE;
}

int label_name_is_keyword(char *label_name, keyword *keywords_table) {
    int i, label_name_len;

    label_name_len = (int)strlen(label_name);

    for (i = 0; i < KEYWORD_TABLE_LENGTH; i++) {
        if (!strncmp(label_name, keywords_table[i].name, label_name_len)) {
            return TRUE;
        }
    }

    return FALSE;
}

int label_name_is_macro(char *label_name, macro_table *macroTable) {
    int i, label_name_len;

    label_name_len = (int)strlen(label_name);

    for (i = 0; i < macroTable->macro_count; i++) {
        if (!strncmp(label_name, macroTable->macros[i]->name, label_name_len)) {
            return TRUE;
        }
    }

    return FALSE;
}

void fill_table_size(label *label_table, int labels_counter) {
    int i;

    for (i = 0; i < labels_counter; i++) {
        label_table[i].size = labels_counter;
    }
}

int string_to_label(char *str, label *label_table) {
    int i, label_num;

    for (i = 0; i < label_table[0].size; i++) {
        if (!strcmp(label_table[i].name, str)) {
            label_num = i;
        }
    }
    return label_num;
}
