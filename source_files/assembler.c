#include "assembler.h"
#include "asm_error.h"


int main(int argc, char **argv) {

    keyword *keyword_table = NULL;
    macro_table *m_table = NULL;
    label_table *_label_table = NULL;
    inst_table *_inst_table = NULL;
    filenames *fnames = NULL;
    int syntax_error_count = 0;
    int i;

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fnames = generate_filenames(argc - 1, argv, fnames);

    if (fnames == NULL) {
        return EXIT_FAILURE;
    }

    /*Initialize the keyword table*/
    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        reset_main(argc - 1, &fnames, NULL, NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    for (i = 0;i < argc - 1;i++) {
        syntax_error_count = 0;
        IC(RESET, 0);
        DC(RESET, 0);

        printf("\n-> Starting assembly of the file '%s.as'\n", fnames->generic[i]);

        /*Initialize the macro table*/
        m_table = pre_assemble(fnames->as[i], fnames->am[i], keyword_table);
        if (m_table == NULL) {
            printf("\nPre assembly of the file '%s.as' has failed.\n", fnames->generic[i]);
            printf("\n------------------------------------------------------------------------------------------------------------------\n");
            continue;
        }
        /* Initialize the label table */
        _label_table = fill_label_table(fnames->am[i], fnames->as[i], m_table, keyword_table, &syntax_error_count);
        if (_label_table == NULL) {
            printf("\nAssembly of the file '%s.as' has failed due to an internal system error.\n", fnames->generic[i]);
            reset_main(argc - 1, &fnames, &m_table, &keyword_table, &_label_table, &_inst_table);
            return EXIT_FAILURE;
        }

        /* Lex the assembly code */
        _inst_table = lex(fnames->am[i], fnames->as[i], _label_table, keyword_table, &syntax_error_count);

        if (syntax_error_count != 0) {
            if (syntax_error_count == 1)
                printf("\nAssembly of the file '%s.as' has failed as a result of 1 syntax error.\n", fnames->generic[i]);

            else
                printf("\nAssembly of the file '%s.as' has failed as a result of %d syntax errors.\n", fnames->generic[i], syntax_error_count);
            fnames->errors[i] = syntax_error_count;
            reset_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            printf("\n------------------------------------------------------------------------------------------------------------------\n");
            continue;
        }

        if (_inst_table == NULL) {
            printf("\nAssembly of the file '%s.as' has failed as a result of an internal system error.\n", fnames->generic[i]);
            reset_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            reset_main(argc - 1, &fnames, &m_table, &keyword_table, &_label_table, &_inst_table);
            return EXIT_FAILURE;
        }

        if (parse(_inst_table, _label_table, keyword_table, fnames->am[i]) != STATUS_OK) {
            reset_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            reset_main(argc - 1, &fnames, &m_table, &keyword_table, &_label_table, &_inst_table);
            return EXIT_FAILURE;
        }
        else {
            printf("\nAssembly of the file '%s.as' completed successfully.\n", fnames->generic[i]);
        }

        reset_main(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
        printf("\n------------------------------------------------------------------------------------------------------------------\n");

    }

    printf("Summary:\n");
    for (i = 0;i < argc - 1;i++) {
        if (fnames->errors[i] != 0) {
            if (fnames->errors[i] == 1)
                printf("'%s.as' --> failure due to 1 syntax error.\n", fnames->generic[i]);
            else
                printf("'%s.as' --> failure due to %d syntax errors.\n", fnames->generic[i], fnames->errors[i]);
        }
        else {
            printf("'%s.as' --> completed assembly\n", fnames->generic[i]);
        }
    }
    reset_main(argc - 1, &fnames, &m_table, &keyword_table, &_label_table, &_inst_table);

    return EXIT_SUCCESS;
}
