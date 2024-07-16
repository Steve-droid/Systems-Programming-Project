#include "symbol_table.h"
#include "utilities.h"


/**
 *@brief This function is used to fill the keywords table with the operation, directive, and register keywords.
 * Here are the keywords that are filled in the table:
 * 1. Registers: r0, r1, r2, r3, r4, r5, r6, r7.
 * 2. Operation keywords: mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, red, prn, jsr, rts, stop.
 * 3. Directive keywords: .data, .string, .entry, .extern.
 * 4. Macro keywords: macr, endmacr.
 * Each keyword has a key that represents its type, and a length that represents the length of the keyword.
 * @return keyword* A pointer to the filled keywords table.
 */
keyword *fill_keywords_table() {
    int i;
    char reg_name[3] = { 0 };
    keyword *keywords_table = NULL;

    /* Operation keywords */
    char *operation_keywords[] = {
       "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
    };
    int operation_keys[] = {
      MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP
    };
    int operation_lengths[] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4
    };

    /* Directive keywords */
    char *directive_keywords[] = {
        ".data", ".string", ".entry", ".extern"
    };
    int directive_keys[] = {
        DATA, STRING, ENTRY, EXTERN
    };
    int directive_lengths[] = {
        5, 7, 6, 7
    };

    /* Macro keywords */
    char *macro_keywords[] = {
        "macr", "endmacr"
    };
    int macro_keys[] = {
        MACR, ENDMACR
    };
    int macro_lengths[] = {
        4, 7
    };

    keywords_table = (keyword *)malloc(KEYWORD_TABLE_LENGTH * sizeof(keyword));
    if (keywords_table == NULL) {
        printf("ERROR-ALLOCATION FAILED\n");
        return NULL;
    }

    /* Register keywords */
    for (i = 0; i < REGISTER_KEYWORDS; i++) {
        sprintf(reg_name, "r%d", i);
        strcpy(keywords_table[i].name, reg_name);
        keywords_table[i].key = R0 + i;
        keywords_table[i].length = 2;
    }


    /* Fill the keywords table */
    for (i = 0; i < OPERATION_KEYWORDS; i++) {
        strcpy(keywords_table[i + REGISTER_KEYWORDS].name, operation_keywords[i]);
        keywords_table[i + REGISTER_KEYWORDS].key = operation_keys[i];
        keywords_table[i + REGISTER_KEYWORDS].length = operation_lengths[i];
    }

    for (i = 0; i < MACRO_KEYWORDS; i++) {
        strcpy(keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS].name, macro_keywords[i]);
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS].key = macro_keys[i];
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS].length = macro_lengths[i];
    }

    for (i = 0; i < DIRECTIVE_KEYWORDS; i++) {
        strcpy(keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS + MACRO_KEYWORDS].name, directive_keywords[i]);
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS + MACRO_KEYWORDS].key = directive_keys[i];
        keywords_table[i + REGISTER_KEYWORDS + OPERATION_KEYWORDS + MACRO_KEYWORDS].length = directive_lengths[i];
    }

    return keywords_table;
}


label *fill_label_table(char *am_filename, macro_table *m_table, keyword *keywords_table) {
    char line[MAX_LINE_LENGTH] = { '\0' }; /* Buffer to hold each line */
    char label_name[MAX_LABEL_LENGTH] = { '\0' }; /* Buffer to hold each label name */
    label_table *_lable_table = NULL;
    size_t lines_in_file = 0;
    size_t next_empty_cell = 0;
    FILE *am_file = NULL;

    /* Open the .am file for reading lines */
    am_file = fopen(am_filename, "r");
    if (am_file == NULL) {
        printf("Error- Failed to open the file '%s'. Exiting...\n", am_filename);
        return NULL;
    }

    /* Print the current .am filename */
    printf("%s\n", am_filename);

    /* Initialize the label table */
    _lable_table = initialize_label_table();
    if (_lable_table == NULL) {
        printf("ERROR- Failed to allocate memory for label table.\nFile name: '%s'.\n Exiting...\n", am_filename);
        return NULL;
    }

    lines_in_file = -1; /* lines start from 0*/

    while (fgets(line, sizeof(line), am_file)) { /* Read every line from the .am file */

        /* Skip empty lines */
        if (is_empty_line(line)) {
            continue;
        }
        /* Skip comment lines */
        if (line[0] == ';') {
            continue;
        }

        lines_in_file++;
        remove_prefix_spaces(line);

        next_empty_cell = _lable_table->size;

        /* Check if the line contains a label */
        strncpy(_lable_table->labels[next_empty_cell]->name, line, MAX_LABEL_LENGTH);
        if (label_name_is_valid(_lable_table, label_name, next_empty_cell, keywords_table, m_table)) {
            fill_label_data_exclude_address(&_lable_table[_lable_table->size], label_name, lines_in_file);
            _lable_table = insert_label(_lable_table, &_lable_table[_lable_table->size]);

            if (_lable_table == NULL) {
                printf("ERROR- Failed to allocate memory for label table.\nFile name: '%s'.\n Exiting...\n", am_filename);
                return NULL;
            }
        }
    }


    fclose(am_file); /* Close the file */
    return _lable_table;
}


label_table *insert_label(label_table *table, label *_lable) {
    if (table->size == table->capacity) {
        table->capacity *= 2;
        table->labels = (label **)realloc(table->labels, table->capacity * sizeof(label *));
        if (table->labels == NULL) {
            free(table);
            return NULL;
        }
    }
    table->labels[table->size] = _lable;
    table->size++;
    return table;
}

label_table *initialize_label_table() {
    label_table *table = NULL;
    if (!(table = (label_table *)malloc(sizeof(label_table))))
        return NULL;
    table->size = 0;
    table->capacity = 1;
    table->labels = (label **)malloc(table->capacity * sizeof(label *));
    if (table->labels == NULL) {
        free(table);
        return NULL;
    }
    return table;
}

label *initialize_label() {
    label *new_label = NULL;
    if (!(new_label = (label *)malloc(sizeof(new_label))))
        return NULL;
    return new_label;
}


bool label_name_is_valid(label *label_table, char *label_name, int table_size, keyword *keywords_table, macro_table *m_table) {
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
                label_name_is_macro(tmp, m_table)) {
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

    /* Fill label name */
    str_cpy_until_char((char *)&any_label->name, line, ':');

    /* Fill label key */
    any_label->key = label_key;
    label_key++;

    /* Fill the line where the label was seen */
    any_label->instruction_line = line_counter;

    /* Fill the address with a default value until the 2nd pass */
    any_label->address = UNDEFINED;
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

int label_name_is_macro(char *label_name, macro_table *m_table) {
    int i, label_name_len;

    label_name_len = (int)strlen(label_name);

    for (i = 0; i < m_table->macro_count; i++) {
        if (!strncmp(label_name, m_table->macros[i]->name, label_name_len)) {
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
