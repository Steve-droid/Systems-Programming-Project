#include "assembler.h"

size_t DC(char *prompt, size_t amount)
{
    static size_t DC = 0;

    if (!strcmp(prompt, "get"))
        return DC;
    if (!strcmp(prompt, "increment")) {
        DC += amount;
    }
    return DC;
}

size_t IC(char *prompt, size_t amount)
{
    static size_t IC = 100;

    if (!strcmp(prompt, "get"))
        return IC;
    if (!strcmp(prompt, "increment")) {
        IC += amount;
    }

    return IC;
}



int main(int argc, char *argv[]) {

    keyword *keyword_table = NULL;
    macro_table *m_table = NULL;
    label_table *_label_table = NULL;
    inst_table *_inst_table = NULL;
    char **am_filenames = NULL;

    /*Initialize the keyword table*/
    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        printf("Error while filling keyword table. Exiting...\n");
        return EXIT_FAILURE;
    }

    /*Initialize the macro table*/
    m_table = fill_macro_table(argc, argv, &am_filenames);
    if (m_table == NULL) {
        printf("Error while filling macro table. Exiting...\n");
        delete_filenames(argc - 1, &am_filenames);
        destroy_keyword_table(&keyword_table);
        return EXIT_FAILURE;
    }
    /* Initialize the label table */
    _label_table = fill_label_table(am_filenames[0], m_table, keyword_table);
    if (_label_table == NULL) {
        printf("Error while filling label table. Exiting...\n");
        delete_filenames(argc - 1, &am_filenames);
        destroy_keyword_table(&keyword_table);
        macro_table_destructor(&m_table);
        return EXIT_FAILURE;
    }

    /* Lex the assembly code */
    _inst_table = lex(am_filenames[0], _label_table, keyword_table);

    if (_inst_table == NULL) {
        printf("Error while lexing the assembly code. Exiting...\n");
        delete_filenames(argc - 1, &am_filenames);
        destroy_keyword_table(&keyword_table);
        destroy_label_table(&_label_table);
        macro_table_destructor(&m_table);
        return EXIT_FAILURE;
    }


    delete_filenames(argc - 1, &am_filenames);
    destroy_keyword_table(&keyword_table);
    destroy_label_table(&_label_table);
    macro_table_destructor(&m_table);
    destroy_instruction_table(&_inst_table);
    return EXIT_SUCCESS;
}
