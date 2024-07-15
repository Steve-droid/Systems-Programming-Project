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
    STATUS_ERROR_INVALID_EXTENSION,
    STATUS_ERROR_MACRO_REDEFINITION,
    STATUS_ERROR_MEMORY_ALLOCATION,
    STATUS_ERROR_MACRO_NOT_FOUND,
    STATUS_ERROR_MACRO_TABLE_IS_EMPTY,
    STATUS_ERROR_MACRO_EXPANDS_TO_NOTHING,
    STATUS_ERROR_WHILE_CREATING_FILENAME
} status;
/**
 *@brief Create a file name given a name and an extension
 *
 * @param initial_name
 * @param extension
 * @return char*
 */
char *create_file_name(char *initial_name, char *extension);
status remove_extension(char *full_filename);
status copy_file_contents(char *src_filename, char *dest_filename);
status remove_whitespace(char *filename);
status backup_files(char ***backup_filenames, int file_count, char *filenames[]);
status initallize_file_names(char *filename, char **am_filename, char **as_filename);
void delete_backup_names(size_t num_files, char **backup_names);
#endif 