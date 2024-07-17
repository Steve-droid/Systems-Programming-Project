
/**
 * @file lexer.h
 * @brief Header file for the lexical analyzer (lexer) component of the assembly code translator.
 *
 * The lexer (lexical analyzer) is a critical component in the process of translating
 * the made-up assembly-like code into machine code or another target format.
 * This file contains the declarations and definitions for the lexer component.
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
#include "label.h"
#include "keyword.h"
#include "util.h"
#include "binary.h"
#include "codegen.h"


 /* Initiallization of the encoded table of the assembly instructions */
/**
 *@brief This function handles the encoding of the assembly instructions into binary code.
 *
 * @param am_filename
 * @param _label_table
 * @param keyword_table
 * @return int*
 */
int **lex(char *am_filename, label_table *_label_table, keyword *keyword_table);



#endif /* DECODING_H */