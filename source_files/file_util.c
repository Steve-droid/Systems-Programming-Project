#include "file_util.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
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

void print_octal(int address, uint16_t number, FILE *file_ptr) {
    char octal[6];
    int index = 5;
    octal[index--] = '\0';

    while (number > 0) {
        octal[index--] = (number & 0x7) + '0';
        number >>= 3;
    }

    while (index >= 0) {
        octal[index--] = '0';
    }
    fprintf(file_ptr, "0%d\t%s\n", address, octal);
}


status create_fname_vec(int file_amount, char ***p1, ...) {
    va_list args;
    char ***ptr;

    va_start(args, p1);
    ptr = p1;

    while (ptr != NULL) {
        (*ptr) = (char **)calloc(file_amount, sizeof(char *));
        if ((*ptr) == NULL) {
            return failure;
        }
        ptr = va_arg(args, char ***);
    }

    va_end(args); /* Clean up the argument list */

    return success;

}

/* Main function that creates '.as', '.am' and '.bk' filenames from command line args */
filenames *generate_filenames(int file_amount, char **argv, filenames *fnames) {
    int i;
    char **as = NULL, **am = NULL, **generic = NULL;
    status _status = failure;
    system_state state;
    system_state *sys_state = &state;


    fnames = (filenames *)malloc(sizeof(filenames));
    if (fnames == NULL) {
        return NULL;
    }

    /* Create an array that holds the amount of errors for each file */
    fnames->errors = calloc(file_amount, sizeof(int));

    fnames->amount = file_amount;

    _status = create_fname_vec(file_amount, &as, &am, &generic, NULL);
    if (_status != success) {
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

        /*If the fp is null, make an 'output' directory */
        if (fp == NULL) {
            mkdir("output", 0755);
            fp = fopen(output_path, mode);

        }

        if (fp == NULL) {
            printf("Error: Trying to open the file '%s' with mode 'w' with no success\n", filename);
            return NULL;
        }

        return fp;
    }

    printf("Invalid use of 'my_fopen' function with mode '%s'- use modes 'r' or 'w' instead\n", mode);


    return NULL;
}