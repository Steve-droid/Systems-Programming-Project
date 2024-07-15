#include <stdio.h>
#include <stdlib.h>
#include "pre_assembler.h"
#include "file_util.h"
#include <string.h>


macro_table *fill_macro_table(int argc, char *argv[], char **am_filename) {
    char *as_filename;
    macro_table *m_table;
    int i;


    char **backup_filenames = (char **)malloc(sizeof(char *) * (argc - 1));
    if (backup_filenames == NULL) err(errno, "Memory allocation error while creating backup file names");

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return NULL;
    }

    printf("Starting file backup...\n");
    if (backup_files(&backup_filenames, argc - 1, argv + 1) != STATUS_OK) {
        printf("File backup did not execute properly. Exiting..");
        return NULL;
    }
    printf("Backup succesful\n");

    printf("Creating .am file... ");
    if (initallize_file_names(argv[1], am_filename, &as_filename) != STATUS_OK) {
        printf("ERROR: .am file creation did not execute properly. Exiting..");
        return NULL;
    }
    printf("Done\n");

    m_table = create_macro_table();

    printf("Starting pre assembly...\n");
    status result = pre_assemble(as_filename, *am_filename, m_table);
    rename("test.as.backup", "test.as");


    switch (result) {
    case STATUS_OK:
        printf("Pre-assembly completed successfully.\n");
        for (i = 0;i < m_table->macro_count;i++) {
            printf("%s\n", m_table->macros[i]->name);
        }
        break;
    case STATUS_ERROR_OPEN_SRC:
        printf("Error: Could not open source file.\n");
        break;
    case STATUS_ERROR_OPEN_DEST:
        printf("Error: Could not open destination file.\n");
        break;
    case STATUS_ERROR_READ:
        printf("Error: Could not read from source file.\n");
        break;
    case STATUS_ERROR_WRITE:
        printf("Error: Could not write to destination file.\n");
        break;
    case STATUS_ERROR_MACRO_REDEFINITION:
        printf("Error: Macro redefinition detected.\n");
        break;
    case STATUS_ERROR_MEMORY_ALLOCATION:
        printf("Error: Memory allocation failed.\n");
        break;
    case STATUS_ERROR_MACRO_NOT_FOUND:
        printf("Error: Macro not found.\n");
        break;
    default:
        printf("Unknown error.\n");
        break;
    }

    free(as_filename);
    delete_backup_names(argc - 1, backup_filenames);
    free(backup_filenames);

    return m_table;
}
