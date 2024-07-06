#ifndef PRE_ASSEMBLER_H_
#define PRE_ASSEMBLER_H_
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macro.h"
#include "vector.h"
#include "file_util.h"

#define as_extention ".as"
#define am_extention ".am"
#define MAX_MACRO_NAME_LENGTH 73
#define MAX_MACRO_AMOUNT 100
#define MAX_LINE_LENGTH 80
#define BUFFER_SIZE 1024


status pre_assemble(char *filename);


#endif