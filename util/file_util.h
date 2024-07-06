#ifndef FILE_UTIL_H
#define FILE_UTIL_H

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
    STATUS_ERROR_OPEN_SRC,
    STATUS_ERROR_OPEN_DEST,
    STATUS_ERROR_READ,
    STATUS_ERROR_WRITE
} status;

char *create_file_name(const char *initial_name, const char *extension);
status copy_file_contents(const char *src_filename, const char *dest_filename);
status remove_whitespace(const char *filename);




#endif 