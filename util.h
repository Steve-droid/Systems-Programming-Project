#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>

/**
 * @brief Trim leading and trailing whitespace from a string
 *
 * @param str The string to trim
 * @return char* A pointer to the first non-whitespace character in the string
 */
char *trim_whitespace(char *str);




#endif 