#include <stdio.h>
#include <stdlib.h>
#include "pre_assembler.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *base_filename = argv[1];
    status result = pre_assemble(base_filename);

    switch (result) {
    case STATUS_OK:
        printf("Pre-assembly completed successfully.\n");
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

    return result == STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
