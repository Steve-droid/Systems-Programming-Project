#include "file_util.h"
#include <stdlib.h>
#include <fcntl.h>
#define ERR -1

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

status copy_file_contents(const char *src_filename, const char *dest_filename) {
    FILE *src_file = fopen(src_filename, "r");
    FILE *dest_file = fopen(dest_filename, "w");
    char buffer[BUFSIZ] = { 0 };
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    bool write_success = true;


    if (src_file == NULL) {
        fprintf(stderr, "Failed to open source file %s\n", src_filename);
        return STATUS_ERROR_OPEN_SRC;
    }

    if (dest_file == NULL) {
        fprintf(stderr, "Failed to open destination file %s\n", dest_filename);
        fclose(src_file);
        return STATUS_ERROR_OPEN_DEST;
    }

    while ((bytes_read = fread(buffer, sizeof(char), BUFSIZ, src_file))) {
        /* Write the bytes you read into the destination file */
        bytes_written = fwrite(buffer, sizeof(char), bytes_read, dest_file);

        /* If we failed to write exactly what we read, data was lost in the process */
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Error writing to file %s\n", dest_filename);
            write_success = false;
            break;
        }
    }

    fclose(src_file);
    fclose(dest_file);

    /* If the operation resulted in a partial copy, we remove the destination file */
    if (write_success == false) {
        remove(dest_filename);
        return STATUS_ERROR_WRITE;
    }

    return STATUS_OK;
}

status remove_whitespace(const char *filename) {
    FILE *file = NULL;
    FILE *tmp_file = NULL;
    char tmp_filename[] = "tmpfileXXXXXX";
    char line[BUFSIZ] = { 0 };
    char *start;
    char *end;
    int temp_file_descriptor = 0;
    int original_line_count = 0;
    int cleaned_line_count = 0;


    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return STATUS_ERROR_OPEN_SRC;
    }

    /*
    * Create a temporary, unique file to write the cleaned lines
    * The return value of mkstemp is -1 if the function encountered an error
     */
    temp_file_descriptor = mkstemp(tmp_filename);
    if (temp_file_descriptor == ERR) {
        fprintf(stderr, "Failed to create temporary file\n");
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }

    /*
    * The fdopen function is used to associate a stream(In our case- a tmp file) with a file descriptor.
    * If succesful, it will return a pointer to a newly created, unique temporary file.
    * This temporary file will hold the lines found in the source file, without leading/trailing whitespace
     */
    tmp_file = fdopen(temp_file_descriptor, "w");

    if (tmp_file == NULL) {
        fprintf(stderr, "Failed to open temporary file\n");
        close(temp_file_descriptor);
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }

    while (fgets(line, sizeof(line), file)) {

        original_line_count++;

        /* Remove leading whitespace */
        start = line;
        while (isspace((unsigned char)*start)) start++;

        /* Remove trailing whitespace */
        end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;

        /* NUll terminate */
        *(end + 1) = '\0';

        /* Write the trimmed line to the tmp file */
        if (fprintf(tmp_file, "%s\n", start) < 0) {
            fprintf(stderr, "Error writing to temporary file\n");
            fclose(file);
            fclose(tmp_file);
            remove(tmp_filename);
            return STATUS_ERROR_WRITE;
        }

        cleaned_line_count++;
    }

    fclose(file);
    fclose(tmp_file);

    /* Verify that the number of lines in both files matches */
    if (original_line_count != cleaned_line_count) {
        fprintf(stderr, "Line count mismatch: original=%d, cleaned=%d\n", original_line_count, cleaned_line_count);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    /* Replace the original file with the cleaned temporary file */
    if (remove(filename) != 0) {
        fprintf(stderr, "Failed to remove original file %s\n", filename);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }
    if (rename(tmp_filename, filename) != 0) {
        fprintf(stderr, "Failed to rename temporary file to %s\n", filename);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    return STATUS_OK;

}