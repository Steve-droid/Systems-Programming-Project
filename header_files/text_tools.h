#ifndef TEXT_TOOLS_H
#define TEXT_TOOLS_H

#include "common.h"
#include "util.h"
#include "keyword.h"
#include "lexer.h"
#include "parser.h"
#include "instruction.h"
#include "binary.h"
/**
 *@brief Removes leading and trailing whitespace from a string.
 *
 * @param str The string to trim.
 * @return char* The trimmed string.
 */
char *trim_whitespace(char *str);
/**
 * @brief Returns a pointer to the character immediately following the label in a line of text.
 *
 * This function takes a line of text, a label table, and the current line number,
 * and returns a pointer to the character in the line immediately following the label
 * (if a label exists on that line).
 *
 * @param line The line of text to process.
 * @param label_table The table containing label information.
 * @param current_line The current line number being processed.
 * @return A pointer to the character immediately following the label, or the original line if no label is found.
 */
char *skip_label_name(buffer_data *_buffer_data, label_table *_label_table);




#endif