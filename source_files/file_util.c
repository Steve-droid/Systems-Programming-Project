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
    system_state s;
    system_state *state = &s;

    file = my_fopen(filename, "r");
    if (file == NULL) return STATUS_ERROR;


    state->as_filename = filename;

    /* Create a temporary file to write the cleaned lines */
    temp_file_descriptor = mkstemp(tmp_filename);
    if (temp_file_descriptor == -1) {
        print_system_error(state, NULL, f5_tmp_file);
        fclose(file);
        return STATUS_ERROR_OPEN_DEST;
    }

    tmp_file = fdopen(temp_file_descriptor, "w");

    if (tmp_file == NULL) {
        print_system_error(state, NULL, f6_open_tmpfile);
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
                print_system_error(state, NULL, f7_write_to_tmp_file);
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
        state->original_line_count = original_line_count;
        state->cleaned_line_count = cleaned_line_count;
        print_system_error(state, NULL, f8_line_mismatch);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
    }

    /* Replace the original file with the cleaned temporary file */
    if (remove(filename) != 0) {

        /*This means that the input file is located at the 'tests' directory, So update the filename */
        strcat(test_path, filename);

        if (remove(test_path) != 0) {
            print_system_error(state, NULL, f9_rmv_original);
            remove(tmp_filename);
            return STATUS_ERROR_WRITE;
        }

        if (rename(tmp_filename, test_path) != 0) {
            print_system_error(state, NULL, f10_rename_tmp);
            remove(tmp_filename);
            return STATUS_ERROR_WRITE;
        }

        return STATUS_OK;


    }
    if (rename(tmp_filename, filename) != 0) {
        print_system_error(state, NULL, f10_rename_tmp);
        remove(tmp_filename);
        return STATUS_ERROR_WRITE;
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
    char **as = NULL, **am = NULL, **generic = NULL;
    status _status = STATUS_ERROR;
    system_state state;
    system_state *sys_state = &state;


    fnames = (filenames *)malloc(sizeof(filenames));
    if (fnames == NULL) {
        return NULL;
    }

    fnames->amount = file_amount;

    _status = create_fname_vec(file_amount, &as, &am, &generic, NULL);
    if (_status != STATUS_OK) {
        free(fnames);
        fnames = NULL;
        return NULL;
    }

    fnames->am = am;
    fnames->as = as;
    fnames->generic = generic;


    /* Create a vector that holds generic filenames- a duplicate of argv */
    for (i = 0;i < file_amount;i++) {
        fnames->generic[i] = my_strdup(argv[i + 1]);
        if (fnames->generic[i] == NULL) {
            sys_state->generic_filename = argv[i + 1];
            print_system_error(sys_state, NULL, m7_generic_creation);
            quit_filename_creation(&fnames);
            return NULL;
        }
    }

    for (i = 0;i < file_amount;i++) {

        fnames->as[i] = add_extension(fnames->generic[i], ".as");

        if (fnames->as[i] == NULL) {
            sys_state->generic_filename = fnames->generic[i];
            print_system_error(sys_state, NULL, f2_as_creation);
            quit_filename_creation(&fnames);
            return NULL;
        }
    }


    for (i = 0;i < file_amount;i++) {
        fnames->am[i] = add_extension(fnames->generic[i], ".am");

        if (fnames->am[i] == NULL) {
            sys_state->generic_filename = fnames ->generic[i];
            print_system_error(sys_state, NULL, f4_am_creation);
            quit_filename_creation(&fnames);
            return NULL;
        }
    }


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