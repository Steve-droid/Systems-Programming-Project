#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include "macro.h"
#include "pre_assembler.h"
#include "util.h"



int main(int argc, char *argv[]) {

    if (argc != 2) {
        errx(1, "Usage: %s <file>", argv[0]);
    }

    pre_assemble(argv[1], "output.am");



    return 0;
}


