
/**
 * @file lexer.h
 * @brief Header file for the lexical analyzer (lexer) component of the assembly code translator.
 *
 *
 * The primary role of the lexer is to:
 * - Read the input source code character by character.
 * - Group characters into meaningful sequences called tokens.
 * - Classify these tokens into categories such as keywords, identifiers, numbers, operators, and punctuation.
 * - Provide a stream of tokens to the parser for further syntactic analysis and translation.
 *
 * The lexer ensures that the source code is broken down into manageable and meaningful components,
 * making it easier for the parser to understand and process the code's structure.
 * It also handles lexical errors by identifying invalid characters and reporting them appropriately.
 */

#ifndef LEXER_H
#define LEXER_H

#include "common.h"
#include "text_util.h"
#include "symbols.h"
#include "instruction.h"
#include "asm_error.h"

#define FIRST_FIELD 0
#define SECOND_FIELD 1
#define THIRD_FIELD 2
#define FOURTH_FIELD 3
#define FIRST_ARG 0
#define SECOND_ARG 1


inst_table *lex(char *am_filename, label_table *_label_table, keyword *keyword_table);

#endif 
