#include "assembler.h"
#include "asm_error.h"



size_t DC(char *prompt, size_t amount)
{
    static size_t _DC = 0;
    if (!strcmp(prompt, "reset")) {
        _DC = 0;
        return _DC;
    }

    if (!strcmp(prompt, "get"))
        return _DC;
    if (!strcmp(prompt, "increment")) {
        _DC += amount;
    }
    return _DC;
}

size_t IC(char *prompt, size_t amount)
{
    static size_t _IC = 100;
    if (!strcmp(prompt, "reset")) {
        _IC = 100;
        return _IC;
    }
    if (!strcmp(prompt, "get"))
        return _IC;
    if (!strcmp(prompt, "increment")) {
        _IC += amount;
    }

    return _IC;
}



int main(int argc, char **argv) {

    keyword *keyword_table = NULL;
    macro_table *m_table = NULL;
    label_table *_label_table = NULL;
    inst_table *_inst_table = NULL;
    char **am_filenames = NULL;
    char **generic_filenames = NULL;
    size_t file_amount = argc - 1;
    char **as_filenames = NULL;
    char **backup_filenames = NULL;
    int i;


    if (file_amount < 1) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    generic_filenames = (char **)calloc(file_amount, sizeof(char *));
    if (generic_filenames == NULL) {
        return EXIT_FAILURE;
    }

    for (i = 0;i < file_amount;i++) {
        generic_filenames[i] = my_strdup(argv[i + 1]);
    }

    as_filenames = (char **)calloc(file_amount, sizeof(char *));
    if (as_filenames == NULL) {
        return EXIT_FAILURE;
    }

    backup_filenames = (char **)calloc(file_amount, sizeof(char *));
    if (backup_filenames == NULL) {
        delete_filenames(file_amount, &as_filenames);
        as_filenames = NULL;
    }

    am_filenames = (char **)calloc(file_amount, sizeof(char *));
    if (am_filenames == NULL) {
        delete_filenames(file_amount, &as_filenames);
        delete_filenames(file_amount, &backup_filenames);
        delete_filenames(file_amount, &am_filenames);
        delete_filenames(file_amount, &generic_filenames);
        as_filenames = NULL;
        backup_filenames = NULL;
        return EXIT_FAILURE;
    }

    for (i = 0;i < file_amount;i++) {

        as_filenames[i] = create_file_name(generic_filenames[i], ".as");

        if (as_filenames[i] == NULL) {
            printf("Error: .as file creation for %s did not execute properly.\nExiting...\n", generic_filenames[i]);
            delete_filenames(file_amount, &as_filenames);
            delete_filenames(file_amount, &backup_filenames);
            delete_filenames(file_amount, &am_filenames);
            delete_filenames(file_amount, &generic_filenames);
            as_filenames = NULL;
            backup_filenames = NULL;
            return EXIT_FAILURE;
        }
    }

    if (duplicate_files(&backup_filenames, file_amount, as_filenames, ".bk") != STATUS_OK) {
        printf("Error: File backup did not execute properly. Exiting..");
        delete_filenames(file_amount, &as_filenames);
        delete_filenames(file_amount, &backup_filenames);
        delete_filenames(file_amount, &am_filenames);
        delete_filenames(file_amount, &generic_filenames);
        as_filenames = NULL;
        backup_filenames = NULL;
        return EXIT_FAILURE;
    }

    for (i = 0;i < file_amount;i++) {
        am_filenames[i] = create_file_name(generic_filenames[i], ".am");

        if (am_filenames[i] == NULL) {
            printf("Error: .am file creation for the file '%s.as' did not execute properly.\nExiting...\n", generic_filenames[i]);
            delete_filenames(file_amount, &as_filenames);
            delete_filenames(file_amount, &backup_filenames);
            delete_filenames(file_amount, &am_filenames);
            delete_filenames(file_amount, &generic_filenames);
            generic_filenames = NULL;
            as_filenames = NULL;
            backup_filenames = NULL;
            return EXIT_FAILURE;
        }
    }

    for (i = 0;i < file_amount;i++) {
        remove(as_filenames[i]);
        rename(backup_filenames[i], strcat(generic_filenames[i], ".as"));
    }



    /*Initialize the keyword table*/
    keyword_table = fill_keyword_table();
    if (keyword_table == NULL) {
        delete_filenames(file_amount, &as_filenames);
        delete_filenames(file_amount, &backup_filenames);
        delete_filenames(file_amount, &am_filenames);

        return EXIT_FAILURE;
    }

    for (i = 0;i < argc - 1;i++) {

        IC("reset", 0);
        DC("reset", 0);

        /*Initialize the macro table*/
        m_table = fill_macro_table(am_filenames[i], as_filenames[i], keyword_table);
        if (m_table == NULL) {
            printf("Pre assembly of the file '%s.as' has failed.\n", argv[1]);
            terminate(0, NULL, NULL, &keyword_table, NULL, NULL);
            break;
        }
        /* Initialize the label table */
        _label_table = fill_label_table(am_filenames[i], m_table, keyword_table);
        if (_label_table == NULL) {
            terminate(argc - 1, NULL, &m_table, NULL, &_label_table, NULL);
            continue;
        }

        /* Lex the assembly code */
        _inst_table = lex(am_filenames[i], _label_table, keyword_table);

        if (_inst_table == NULL) {
            terminate(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            continue;
        }

        if (parse(_inst_table, _label_table, keyword_table, am_filenames[i]) != STATUS_OK) {
            terminate(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);
            printf("Assembly of the file '%s' has failed.\n", generic_filenames[i]);
            continue;
        }

        printf("Assembly of the file '%s' completed successfully.\n", generic_filenames[i]);
        terminate(argc - 1, NULL, &m_table, NULL, &_label_table, &_inst_table);

    }

    printf("No more files to process.\n");
    terminate(argc - 1, &am_filenames, &m_table, &keyword_table, &_label_table, &_inst_table);
    delete_filenames(file_amount, &as_filenames);
    delete_filenames(file_amount, &generic_filenames);
    delete_filenames(file_amount, &backup_filenames);
    backup_filenames = NULL;

    return EXIT_SUCCESS;
}
