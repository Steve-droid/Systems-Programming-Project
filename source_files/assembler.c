#include "assembler.h"
#include "asm_error.h"



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
    data_image *_data_image = NULL;


    /*Initialize the keyword table*/
    printf("Creating keyword table... ");
    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        printf("Error while filling keyword table. Exiting...\n");
        return EXIT_FAILURE;
    }
    printf("Done\n");


    /*Initialize the macro table*/
    m_table = fill_macro_table(argc, argv, &am_filenames);
    if (m_table == NULL) {
        printf("Error while filling macro table. Exiting...\n");
        terminate(0, NULL, NULL, &keyword_table, NULL, NULL);
        return EXIT_FAILURE;
    }
    /* Initialize the label table */
    printf("Scanning labels in '%s'... ", am_filenames[0]);
    _label_table = fill_label_table(am_filenames[0], m_table, keyword_table);
    if (_label_table == NULL) {
        printf("Error while filling label table. Exiting...\n");
        terminate(argc - 1, &am_filenames, &m_table, &keyword_table, &_label_table, NULL);
        return EXIT_FAILURE;
    }
    printf("Done\n");

    /* Lex the assembly code */
    printf("Lexing the file '%s'...", am_filenames[0]);
    _inst_table = lex(am_filenames[0], _label_table, keyword_table);

    if (_inst_table == NULL) {
        printf("Error while lexing the assembly code. Exiting...\n");
        terminate(argc - 1, &am_filenames, &m_table, &keyword_table, &_label_table, &_inst_table);
        return EXIT_FAILURE;
    }
    printf("Done\n");

    printf("Parsing the file '%s'... ", am_filenames[0]);
    if (parse(_inst_table, _label_table, keyword_table, am_filenames[0]) != STATUS_OK) {
        printf("Error while parsing the assembly code. Exiting...\n");
        terminate(argc - 1, &am_filenames, &m_table, &keyword_table, &_label_table, &_inst_table);
        return EXIT_FAILURE;
    }

    printf("Done\n");

    printf("Assembly of the file '%s' completed successfully. Exiting...\n", am_filenames[0]);
    terminate(argc - 1, &am_filenames, &m_table, &keyword_table, &_label_table, &_inst_table);
    return EXIT_SUCCESS;

}
