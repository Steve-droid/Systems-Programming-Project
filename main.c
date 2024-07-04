#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>


int main(int argc, char *argv[]) {

    FILE *as_file = NULL;
    FILE *am_file = NULL;

    if (argc != 2) {
        errx(1, "Usage: %s <file_name>", argv[0]);
    }




    return 0;
}