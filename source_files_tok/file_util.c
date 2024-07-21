#include "file_util.h"

/*---------------File Utilities---------------*/
char *create_file_name(char *initial_filename, char *extension) {
    size_t initial_name_len = strlen(initial_filename);
    size_t extension_len = strlen(extension);
    size_t new_filename_len = initial_name_len + extension_len; /*Length of the new filename*/
    char *new_filename = NULL;

    /* Allocate memory for the new filename */
    new_filename = (char *)calloc(new_filename_len + 1, sizeof(char));
    if (new_filename == NULL) {
        printf("Memory allocation error while creating a new filename for %s\nExiting...\n", initial_filename);
        return NULL;
    }

    /* Concatenate the initial name and the extension */
    strcpy(new_filename, initial_filename);
    strcat(new_filename, extension);

    return new_filename;
}

status remove_file_extension(char *full_filename, char **generic_filename) {

    size_t full_filename_length = 0;
    char *extension = NULL;

    if (full_filename == NULL) {
        printf("Trying to remove extention from an empty filename\nExiting...\n");
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    full_filename_length = strlen(full_filename);

    *(generic_filename) = (char *)calloc((full_filename_length + 1), sizeof(char));

    if (*(generic_filename) == NULL) {
        printf("Memory allocation error while creating generic filename for %s\nExiting...\n", full_filename);
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    strcpy(*(generic_filename), full_filename);
    extension = strstr(*(generic_filename), ".");


    if (extension == NULL) {
        printf("Trying to remove extention from a filename with no extention\nExiting...\n");
        free(*generic_filename);
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    *extension = '\0'; /*Null terminate the generic filename*/
    return STATUS_OK;
}

status copy_file_contents(char *src_filename, char *dest_filename) {
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

status remove_whitespace(char *filename) {
    FILE *file;
    FILE *tmp_file;
    char *start;
    char *end;
    char *tmp;
    size_t i;
    char tmp_filename[] = "tmpfileXXXXXX";
    int temp_file_descriptor;
    char line[BUFSIZ];
    int original_line_count = 0;
    int cleaned_line_count = 0;
    bool line_contains_only_whitespace = false;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file %s\n", filename);
        return STATUS_ERROR_OPEN_SRC;
    }

    /* Create a temporary file to write the cleaned lines */
    temp_file_descriptor = mkstemp(tmp_filename);
    if (temp_file_descriptor == -1) {
        fprintf(stderr, "Failed to create temporary file\n");
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }
    tmp_file = fdopen(temp_file_descriptor, "w");

    if (tmp_file == NULL) {
        fprintf(stderr, "Failed to open temporary file\n");
        close(temp_file_descriptor);
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }

    /* Count lines in the original file and write cleaned lines to the temporary file */
    while (fgets(line, sizeof(line), file)) {

        start = line;
        end = start + strlen(start) - 1;
        line_contains_only_whitespace = true;

        /*Check if the line contains only whitespace. If so, skip the line*/
        for (i = 0, tmp = start;line_contains_only_whitespace == true && i < strlen(start) && tmp != NULL;i++, tmp++) {
            if (!isspace(*tmp)) {
                line_contains_only_whitespace = false;
                break;
            }
        }

        if (line_contains_only_whitespace) continue;



        original_line_count++;

        /* Remove leading whitespace */
        while (isspace((unsigned char)*start)) {
            start++;
        }

        /* Remove trailing whitespace */
        while (end > start && isspace((unsigned char)*end)) {
            end--;
        }
        *(end + 1) = '\0';  /* Null-terminate the string*/

        /* Only write non-empty lines to the temporary file */
        if (*start != '\0') {
            if (fprintf(tmp_file, "%s\n", start) < 0) {
                fprintf(stderr, "Error writing to temporary file\n");
                fclose(file);
                fclose(tmp_file);
                remove(tmp_filename);
                return STATUS_ERROR_WRITE;
            }
            cleaned_line_count++;
        }
    }

    fclose(file);
    fclose(tmp_file);

    /* Verify that the number of lines matches */
    if (original_line_count != cleaned_line_count) {
        fprintf(stderr, "Line count mismatch: original=%d, cleaned=%d\n", original_line_count, cleaned_line_count);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    /* Replace the original file with the cleaned temporary file */
    if (remove(filename) != 0) {
        fprintf(stderr, "Failed to remove original file %s: %s\n", filename, strerror(errno));
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }
    if (rename(tmp_filename, filename) != 0) {
        fprintf(stderr, "Failed to rename temporary file to %s: %s\n", filename, strerror(errno));
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    return STATUS_OK;
}

status duplicate_files(char ***backup_filenames, int file_count, char *filenames[], char *extention) {
    char *current_filename = filenames[0];
    char *filename_copy = NULL;
    int i;
    int backup_filenames_count = 0;
    int current_filename_len = 0;
    int filename_copy_len = 0;

    if (filenames == NULL || file_count < 1) {
        printf("Attempting to backup an empty file list. Exiting...");
        exit(EXIT_FAILURE);
    }

    for (i = 0;i < file_count;i++) {
        current_filename = filenames[i];

        if (current_filename == NULL) {
            printf("Error while backing up argument names. Original filename is NULL. Exiting...");
            return STATUS_ERROR;
        }

        current_filename_len = strlen(current_filename);
        filename_copy_len = current_filename_len + strlen(extention) - 1;
        filename_copy = (char *)calloc(filename_copy_len * 5, sizeof(char));
        if (filename_copy == NULL) {
            printf("Memory allocation error while creating a backup %s filename", extention);
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
        strcpy(filename_copy, current_filename);
        strcat(filename_copy, extention);

        printf("Copying %s file into %s%s... ", current_filename, current_filename, extention);
        if (filename_copy == NULL) {
            printf("Error while backing up files. Removing created backups...\n");
            for (i = 0;i < backup_filenames_count;i++) {
                if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
            }
            printf("Done. Exiting...\n");
            return STATUS_ERROR;
        }
        printf("Done\n");

        if (copy_file_contents(current_filename, filename_copy) != STATUS_OK) {
            printf("Error while creating a backup file for %s. Exiting...\n", current_filename);
            printf("Error while backing up files. Removing created backups...\n");
            for (i = 0;i < backup_filenames_count;i++) {
                if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
            }
            printf("Done. Exiting...\n");
            return STATUS_ERROR_WHILE_CREATING_FILENAME;
        }

        *backup_filenames[i] = filename_copy;
        backup_filenames_count++;
    }

    if (backup_filenames_count != file_count) {
        printf("Error while backing up files. Number of files in backup is different from the original number of files. ");
        printf("\nRemoving backup filenames...");
        for (i = 0;i < backup_filenames_count;i++) {
            if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
        }
        printf("Done.\nExiting...\n");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

status delete_filenames(size_t file_amount, char **filenames) {
    size_t i;
    if (filenames == NULL) {
        printf("ERROR: Attempted to free filenames but filename array is empty. Exiting...");
        return STATUS_ERROR;
    }

    printf("Deleting filenames...\n");
    for (i = 0;i < file_amount;i++) {

        if (filenames[i] != NULL) {
            printf("Deleting filename %s... ", filenames[i]);
            free(filenames[i]);
            printf("Done\n");
        }
    }

    return STATUS_OK;
}
