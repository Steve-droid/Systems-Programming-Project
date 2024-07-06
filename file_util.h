#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>


typedef enum {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_ERROR_OPEN_SRC,
    STATUS_ERROR_OPEN_DEST,
    STATUS_ERROR_READ,
    STATUS_ERROR_WRITE,
    STATUS_ERROR_MACRO_REDEFINITION,
    STATUS_ERROR_MEMORY_ALLOCATION,
    STATUS_ERROR_MACRO_NOT_FOUND
} status;

char *create_file_name(char *initial_name, char *extension);
status copy_file_contents(char *src_filename, char *dest_filename);
status remove_whitespace(char *filename);




#endif 