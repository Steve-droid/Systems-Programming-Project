#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include "macro.h"
#include "util/file_util.h"




int main() {

    const char *initial_name = "input";
    const char *extension = ".as";
    char *new_file_name = create_file_name(initial_name, extension);

    if (new_file_name != NULL) {
        printf("New file name: %s\n", new_file_name);
        free(new_file_name); // Remember to free the allocated memory
    }



    return 0;
}


