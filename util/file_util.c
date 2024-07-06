#include "file_util.h"

char *create_file_name(const char *initial_filename, const char *extension) {
    size_t initial_name_len = strlen(initial_filename);
    size_t extension_len = strlen(extension);
    size_t new_filename_len = initial_name_len + extension_len; /*Length of the new filename*/
    char *new_filename = NULL;

    /* Allocate memory for the new filename */
    new_filename = (char *)malloc((new_filename_len + 1));
    if (new_filename == NULL) err(errno, "Memory allocation failed\n");

    /* Concatenate the initial name and the extention */
    strcpy(new_filename, initial_filename);
    strcat(new_filename, extension);

    return new_filename;

}