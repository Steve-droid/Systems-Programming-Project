#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>

char *create_file_name(const char *initial_name, const char *extension);
void copy_file_contents(const char *src_filename, const char *dest_filename);




#endif 