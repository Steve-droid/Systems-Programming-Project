#include "file_util.h"
#include <dirent.h>
#define MAX_PATH_LENGTH 30

char *add_extension(char *initial_filename, char *extension) {
    size_t initial_name_len = strlen(initial_filename);
    size_t extension_len = strlen(extension);
    size_t new_filename_len = initial_name_len + extension_len; /*Length of the new filename*/
    char *new_filename = NULL;


    /* Allocate memory for the new filename */
    new_filename = (char *)calloc(new_filename_len + 1, sizeof(char));
    if (new_filename == NULL) {
        return NULL;
    }

    /* Concatenate the initial name and the extension */
    strcpy(new_filename, initial_filename);
    strcat(new_filename, extension);
    return new_filename;
}

status remove_file_extension(char **full_filename, char **generic_filename) {
    syntax_state *state = NULL;
    size_t full_filename_length = 0;
    char *extension = NULL;

    state = create_syntax_state();
    if (state == NULL) return STATUS_ERROR_MEMORY_ALLOCATION;

    if (full_filename == NULL || (*full_filename) == NULL) {
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    if (generic_filename == NULL) {
        return STATUS_ERROR_INVALID_EXTENSION;
    }


    full_filename_length = strlen((*full_filename));

    *(generic_filename) = (char *)calloc((full_filename_length + 1), sizeof(char));

    if (*(generic_filename) == NULL) {
        state->tmp_arg = full_filename;
        my_perror(state, m8_rmv_ext);
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    strcpy((*generic_filename), (*full_filename));
    extension = strstr(*(generic_filename), ".");


    if (extension == NULL) {
        state->tmp_arg = full_filename;
        my_perror(state, e52_inval_ext);
        free(*generic_filename);
        (*generic_filename) = NULL;
        return STATUS_ERROR_INVALID_EXTENSION;
    }

    *extension = '\0'; /*Null quit_filename_creation the generic filename*/
    state->tmp_arg = NULL;
    destroy_syntax_state(&state);
    return STATUS_OK;
}

status copy_file_contents(char *src_filename, char *dest_filename) {
    FILE *src_file = NULL;
    FILE *dest_file = NULL;
    char buffer[BUFSIZ] = { 0 };
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    int write_success = true;

    if (src_filename == NULL || dest_filename == NULL) {
        printf("Error:  Trying to copy to/from a null file path\n");
        return STATUS_ERROR;
    }



    src_file = my_fopen(src_filename, "r");
    if (src_file == NULL) return STATUS_ERROR;

    dest_file = my_fopen(dest_filename, "w");

    if (dest_file == NULL) {
        fclose(src_file);
        return STATUS_ERROR;
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

status remove_whitespace_from_file(char *filename) {
    FILE *file;
    FILE *tmp_file;
    char *start;
    char *end;
    char *tmp;
    size_t i;
    char tmp_filename[] = "tmpfileXXXXXX";
    char test_path[MAX_LINE_LENGTH] = "input/";
    int temp_file_descriptor;
    char line[BUFSIZ];
    int original_line_count = 0;
    int cleaned_line_count = 0;
    int line_contains_only_whitespace = false;
    syntax_state *state = NULL;

    file = my_fopen(filename, "r");
    if (file == NULL) return STATUS_ERROR;

    state = create_syntax_state();
    if (state == NULL) return STATUS_ERROR_MEMORY_ALLOCATION;

    /* Create a temporary file to write the cleaned lines */
    temp_file_descriptor = mkstemp(tmp_filename);
    if (temp_file_descriptor == -1) {
        
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

        if (line_contains_only_whitespace == true) continue;



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

        /*This means that the input file is located at the 'tests' directory, So update the filename */
        strcat(test_path, filename);

        if (remove(test_path) != 0) {
            fprintf(stderr, "Failed to remove original file %s: %s\n", filename, strerror(errno));
            remove(tmp_filename);
            return STATUS_ERROR_WRITE;
        }

        if (rename(tmp_filename, test_path) != 0) {
            fprintf(stderr, "Failed to rename temporary file to %s: %s\n", filename, strerror(errno));
            remove(tmp_filename);
            return STATUS_ERROR_WRITE;
        }

        return STATUS_OK;


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
        return STATUS_ERROR;
    }

    for (i = 0;i < file_count;i++) {
        current_filename = filenames[i];

        if (current_filename == NULL) {
            printf("Error while backing up filenames. Original filename is NULL. Exiting...");
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

        if (filename_copy == NULL) {
            printf("Error while backing up files. Removing created backups...\n");
            for (i = 0;i < backup_filenames_count;i++) {
                if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
            }
            return STATUS_ERROR;
        }

        if (copy_file_contents(current_filename, filename_copy) != STATUS_OK) {
            printf("Error while creating a backup file for %s\n", current_filename);
            printf("Error while backing up files. Removing created backups...\n");

            free(filename_copy);

            return STATUS_ERROR_WHILE_CREATING_FILENAME;
        }

        (*backup_filenames)[i] = filename_copy;
        backup_filenames_count++;
    }

    if (backup_filenames_count != file_count) {
        printf("Error while backing up files. Number of files in backup is different from the original number of files.\n ");
        printf("\nRemoving backup filenames...");
        for (i = 0;i < backup_filenames_count;i++) {
            if (*backup_filenames[i] != NULL) free(*backup_filenames[i]);
        }
        printf("Done.\nExiting...\n");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

status create_fname_vec(int file_amount, char ***p1, ...) {
    va_list args;
    char ***ptr;

    va_start(args, p1);
    ptr = p1;

    while (ptr != NULL) {
        (*ptr) = (char **)calloc(file_amount, sizeof(char *));
        if ((*ptr) == NULL) {
            return STATUS_ERROR_MEMORY_ALLOCATION;
        }
        ptr = va_arg(args, char ***);
    }

    va_end(args); /* Clean up the argument list */

    return STATUS_OK;

}

/* Main function that creates '.as', '.am' and '.bk' filenames from command line args */
filenames *generate_filenames(int file_amount, char **argv, filenames *fnames) {
    int i;
    char **as = NULL, **am = NULL, **generic = NULL, **backup = NULL;
    status _status = STATUS_ERROR;
    char output_path[MAX_LINE_LENGTH] = "output/";
    syntax_state *state = NULL;

    state = create_syntax_state();

    if (state == NULL) return NULL;

    fnames = (filenames *)malloc(sizeof(filenames));
    if (fnames == NULL) return NULL;

    fnames->amount = file_amount;

    _status = create_fname_vec(file_amount, &as, &am, &generic, &backup, NULL);
    if (_status != STATUS_OK) {
        free(fnames);
        fnames = NULL;
        return NULL;
    }

    fnames->am = am;
    fnames->as = as;
    fnames->generic = generic;
    fnames->backup = backup;
    _status = STATUS_ERROR;


    /* Create a vector that holds generic filenames- a duplicate of argv */
    for (i = 0;i < file_amount;i++) {
        fnames->generic[i] = my_strdup(argv[i + 1]);
        if (fnames->generic[i] == NULL) {
            state->generic_filename = argv[i + 1];
            my_perror(state, m7_generic_creation);
        }
    }

    for (i = 0;i < file_amount;i++) {

        fnames->as[i] = add_extension(fnames->generic[i], ".as");

        if (fnames->as[i] == NULL) {
            state->generic_filename = fnames->generic[i];
            my_perror(state, f2_as_creation);
            quit_filename_creation(&fnames);
            return NULL;
        }
    }
    _status = duplicate_files(&fnames->backup, file_amount, fnames->as, ".bk");
    if (_status != STATUS_OK) {
        my_perror(state, f3_backup);
        quit_filename_creation(&fnames);
        return NULL;
    }

    for (i = 0;i < file_amount;i++) {
        fnames->am[i] = add_extension(fnames->generic[i], ".am");

        if (fnames->am[i] == NULL) {
            state->generic_filename = fnames ->generic[i];
            my_perror(state, f4_am_creation);
            quit_filename_creation(&fnames);
            return NULL;
        }
    }

    for (i = 0;i < file_amount;i++) {
        remove(strcat(output_path, fnames->backup[i]));
        strcpy(output_path, "output/");
        free(fnames->backup[i]);
    }

    for (i = 0;i < file_amount;i++) {
        fnames->backup[i] = NULL;
    }

    free(fnames->backup);
    destroy_syntax_state(&state);
    state = NULL;
    return fnames;
}

/* Basic util functions used for file handling */
void close_files(FILE *p1, ...) {
    va_list args;
    FILE *current_file;

    va_start(args, p1);

    current_file = p1;
    while (current_file != NULL) {
        fclose(current_file);
        current_file = va_arg(args, FILE *);
    }

    va_end(args);
}

void free_filenames(char *p1, ...) {
    va_list args;
    char *current_filename;
    va_start(args, p1);

    current_filename = p1;
    while (current_filename != NULL) {
        free(current_filename);
        current_filename = va_arg(args, char *);
    }
    va_end(args);
}

FILE *my_fopen(const char *filename, const char *mode) {
    FILE *fp = NULL;
    char input_path[MAX_LINE_LENGTH] = "input/";
    char output_path[MAX_LINE_LENGTH] = "output/";

    if (filename == NULL || mode == NULL) return NULL;

    strcat(input_path, filename);
    strcat(output_path, filename);

    if (strcmp(mode, "r") == 0) {
        fp = fopen(filename, mode);
        if (fp == NULL) {
            fp = fopen(input_path, mode);
            if (fp == NULL) {
                fp = fopen(output_path, mode);
                if (fp == NULL) {
                    printf("Error: Trying to open the file '%s' with mode 'r' but the file cannot be found in the project directory\n", filename);
                    return NULL;
                }
            }
        }

        return fp;
    }

    if (strcmp(mode, "w") == 0) {
        fp = fopen(output_path, mode);
        if (fp == NULL) {
            printf("Error: Trying to open the file '%s' with mode 'w' with no success\n", filename);
            return NULL;
        }

        return fp;
    }

    printf("Invalid use of 'my_fopen' function with mode '%s'- use modes 'r' or 'w' instead\n", mode);


    return NULL;
}