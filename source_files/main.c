#include "common.h"
#include "codegen.h"
#include "util.h"
#include "parser.h"
#include "error.h"
#include "label.h"
#include "pre_assembler.h"
#include "lexer.h"


int main(int argc, char *argv[])
{

    keyword *keyword_table = NULL;
    macro_table *m_table = NULL;
    label_table *_label_table = NULL;
    int *encoded_array = NULL;
    char **am_filenames = NULL;

    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        printf("Error while filling keyword table. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    m_table = fill_macro_table(argc, argv, &am_filenames);
    if (m_table == NULL) {
        printf("Error while filling macro table. Exiting...\n");
        free(keyword_table);
        exit(EXIT_FAILURE);
    }
    print_macro_table(m_table);
    _label_table = fill_label_table(am_filenames[0], m_table, keyword_table);
    if (_label_table == NULL) {
        printf("Error while filling label table. Exiting...\n");
        delete_filenames(argc - 1, am_filenames);
        macro_table_destructor(m_table);
        free(keyword_table);
        free(_label_table);
        free(encoded_array);
        exit(EXIT_FAILURE);
    }

    print_label_table(_label_table);
    encoded_array = proccess_assembly_code(am_filenames[0], _label_table, keyword_table);
    if (encoded_array == NULL) {
        printf("Error while decoding. Exiting...\n");
        delete_filenames(argc - 1, am_filenames);
        macro_table_destructor(m_table);
        free(keyword_table);
        free(_label_table);
        free(encoded_array);
        exit(EXIT_FAILURE);
    }
    printf("\nOUTPUT AFTER DECODING LABEL ADDRESS:\n\n");
    print_array_in_binary(encoded_array);

    delete_filenames(argc - 1, am_filenames);
    macro_table_destructor(m_table);
    free(keyword_table);
    free(_label_table);
    free(encoded_array);

    return 0;
}
