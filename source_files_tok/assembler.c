#include "assembler.h"

size_t DC(char *prompt)
{
    static size_t DC = 0;

    if (!strcmp(prompt, "get"))
        return DC;
    if (!strcmp(prompt, "increment")) {
        DC++;
    }
    return DC;
}

size_t IC(char *prompt)
{
    static size_t IC = 0;

    if (!strcmp(prompt, "get"))
        return IC;
    if (!strcmp(prompt, "increment")) {
        IC++;
    }
    return IC;
}



int main(int argc, char *argv[]) {

    keyword *keyword_table = NULL;
    macro_table *m_table = NULL;
    label_table *_label_table = NULL;
    inst_table *_inst_table = NULL;
    bin_table *_bin_table = NULL;
    char **am_filenames = NULL;

    /*Initialize the keyword table*/
    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        printf("Error while filling keyword table. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    /*Initialize the macro table*/
    m_table = fill_macro_table(argc, argv, &am_filenames);
    if (m_table == NULL) {
        printf("Error while filling macro table. Exiting...\n");
        delete_filenames(argc - 1, &am_filenames);
        destroy_keyword_table(&keyword_table);
        exit(EXIT_FAILURE);
    }
    /* Initialize the label table */
    _label_table = fill_label_table(am_filenames[0], m_table, keyword_table);
    if (_label_table == NULL) {
        printf("Error while filling label table. Exiting...\n");
        delete_filenames(argc - 1, &am_filenames);
        destroy_keyword_table(&keyword_table);
        macro_table_destructor(&m_table);
        exit(EXIT_FAILURE);
    }
    printf("Label table before assigning addresses:\n");
    print_label_table(_label_table);

    /* Process the assembly code */
    _inst_table = lex(am_filenames[0], _label_table, keyword_table);

    if (_inst_table == NULL) {
        printf("Error while lexing the assembly code. Exiting...\n");
        delete_filenames(argc - 1, &am_filenames);
        destroy_keyword_table(&keyword_table);
        destroy_label_table(&_label_table);
        macro_table_destructor(&m_table);
        exit(EXIT_FAILURE);
    }

    /* _bin_table = parse(_inst_table, _label_table, keyword_table);

    if (_bin_table == NULL) {
        printf("Error while parsing the assembly code. Exiting...\n");
        delete_filenames(argc - 1, am_filenames);
        macro_table_destructor(m_table);
        label_table_destroy(&_label_table);
        destroy_instruction_table(_inst_table);
        free(keyword_table);
        free(_label_table);
        free(_inst_table);
        free(_bin_table);
        exit(EXIT_FAILURE);
    }
*/

/* Free allocated memory */
    delete_filenames(argc - 1, &am_filenames);
    destroy_keyword_table(&keyword_table);
    destroy_label_table(&_label_table);
    macro_table_destructor(&m_table);
    destroy_instruction_table(&_inst_table);
    return 0;
}
