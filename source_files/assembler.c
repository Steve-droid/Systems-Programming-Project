#include "assembler.h"
#include "asm_error.h"


int main(int argc, char **argv) {

    keyword *keyword_table = NULL;
    macro_table *m_table = NULL;
    label_table *_label_table = NULL;
    inst_table *_inst_table = NULL;
    filenames *fnames = NULL;
    int i;

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fnames = generate_filenames(argc - 1, argv, fnames);

    if (fnames == NULL) {
        printf("Failed to create filenames. Exiting...\n");
        return EXIT_FAILURE;
    }

    /*Initialize the keyword table*/
    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        /**
         *@todo Free all memory
         *
        */

        return EXIT_FAILURE;
    }

    for (i = 0;i < argc - 1;i++) {

        IC("reset", 0);
        DC("reset", 0);

        /*Initialize the macro table*/
        m_table = fill_macro_table(fnames->am[i], fnames->as[i], keyword_table);
        if (m_table == NULL) {
            printf("Pre assembly of the file '%s.as' has failed.\n", argv[1]);
            quit_main(0, NULL, NULL, &keyword_table, NULL, NULL);
            break;
        }
        /* Initialize the label table */
        _label_table = fill_label_table(fnames->am[i], m_table, keyword_table);
        if (_label_table == NULL) {
            quit_main(argc - 1, NULL, &m_table, NULL, &_label_table, NULL);
            continue;
        }

        /* Lex the assembly code */
        _inst_table = lex(fnames->am[i], _label_table, keyword_table);

        if (_inst_table == NULL) {
            quit_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            continue;
        }

        if (parse(_inst_table, _label_table, keyword_table, fnames->am[i]) != STATUS_OK) {
            quit_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            printf("Assembly of the file '%s' has failed.\n", fnames->generic[i]);
            continue;
        }

        printf("Assembly of the file '%s' completed successfully.\n", fnames->generic[i]);
        quit_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);

    }

    printf("No more files to process.\n");
    quit_main(argc - 1, &fnames, &m_table, &keyword_table, &_label_table, &_inst_table);

    return EXIT_SUCCESS;
}
