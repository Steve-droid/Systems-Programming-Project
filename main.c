#include <stdio.h>
#include <stdlib.h>
#include "util/file_util.h"

int main(int argc, char *argv[]) {

    const char *filename = argv[1];
    status result;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Remove whitespace from each line of the file and check the status */
    result = remove_whitespace(filename);
    switch (result) {
    case STATUS_OK:
        printf("Whitespace removed successfully.\n");
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
    default:
        printf("Unknown error.\n");
        break;
    }

    return result == STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
