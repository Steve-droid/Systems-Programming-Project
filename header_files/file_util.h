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


#define MAX_LABEL_LENGTH 31 
#define MAX_MACRO_NAME 20
#define UNDEFINED -1
#define ERR -1

 /**
 *@brief Print a number in octal format to the .ob file
 *
 * @param address Address of the number in the memory- printed alongside and to the left the octal number
 * @param number The number to print in octal format
 * @param file_ptr A pointer to the .ob file to print to
 */
void print_octal(int address, uint16_t number, FILE *file_ptr);

/**
 *@brief Add an extention to a generic file name
 *
 * @param initial_name The initial name of the file
 * @param extension The extension to add to the file
 * @return char* A pointer to the new file name with the extension
 */
char *add_extension(char *initial_name, char *extension);

/**
 *@brief Generates arrays of file pointers- .as, .am and generic filenames
 *
 * @param file_amount The number of files to process
 * @param argv The array of file names passed as command line arguments
 * @param fnames The filenames data structure to store the file pointers
 * @return filenames* A pointer to the filenames data structure containing the file pointers
 */
filenames *generate_filenames(int file_amount, char **argv, filenames *fnames);

/**
 *@brief Search for a file in the 'input/' and 'output/' directories and only then call fopen
 * This functions allows us to create all generated files in the 'output/' directory and read all input files from the 'input/' directory
 * If an input file is not found in the 'input/' directory, the function will search for it in the current directory, so both cases are covered
 * @param filename The name of the file to open
 * @param mode The mode to open the file in
 * @return FILE* A pointer to the file
 */
FILE *my_fopen(const char *filename, const char *mode);

/**
 *@brief Free an unknown number of filenames(variadic function)
 *
 * @param p1 The first filename to free
 */
void free_filenames(char *p1, ...);

/**
 *@brief Close an unknown number of files(variadic function)
 *
 * @param p1 The first file to close
 */
void close_files(FILE *p1, ...);

#endif
