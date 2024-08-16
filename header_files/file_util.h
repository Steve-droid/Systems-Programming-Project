/**
 *@file general_utils.h
 * @brief This file contains the utilities functions and definitions
 * Utilities functions are functions that are used in multiple files and are not specific to any file
 * Includes functions for file manipulation, string manipulation, and other general functions
 * @version 0.1
 * @date 2024-07-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include "data_structs.h"
#include "symbols.h"
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>


#define MAX_LABEL_LENGTH 31 /* max label len is 31 , + 1 for ':' , + 1 for '\0' */ /* Identify each label separately without fear of a word whose maximum size is -1+2^15 */
#define MAX_MACRO_NAME 20
#define UNDEFINED -1
#define ERR -1


 /* File Utilities */
void print_octal(int address, uint16_t number, FILE *file_ptr);
void free_filenames(char *p1, ...);
void close_files(FILE *p1, ...);
char *add_extension(char *initial_name, char *extension);
filenames *generate_filenames(int file_amount, char **argv, filenames *fnames);
FILE *my_fopen(const char *filename, const char *mode);

#endif
