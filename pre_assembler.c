#include "pre_assembler.h"


void pre_assemble(char *src_file_name, const char *output_file_path) {

    char line_buffer[MAX_LINE_LENGTH];      /* For reading macro lines */
    unsigned int output_file_name_length;   /* Length of the output file name */
    unsigned int as_file_name_length = strlen(src_file_name);       /* Length of the .as file name */
    unsigned int am_file_name_length = strlen(src_file_name);      /* Length of the .am file name */
    char *as_file_name = strcat(strncpy(malloc(as_file_name_length + 1), src_file_name, as_file_name_length), as_extention); /* The .as file name */
    char *am_file_name = strcat(strncpy(malloc(am_file_name_length + 1), src_file_name, am_file_name_length), am_extention); /* The .am file name */





}

