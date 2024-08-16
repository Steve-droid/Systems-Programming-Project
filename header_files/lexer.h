
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

#include "data_structs.h"
#include "text_util.h"
#include "symbols.h"
#include "instruction.h"
#include "asm_error.h"


inst_table *lex(char *am_filename, char *as_filename, label_table *_label_table, keyword *keyword_table, int *syntax_error_count);
status init_lexer(char *am_filename, char *as_filename, label_table *_label_table, keyword *keyword_table);
status generate_tokens(syntax_state *state, keyword *keyword_table, label_table *_label_table);
status assign_data(syntax_state *state, label_table *_label_table, keyword *keyword_table);
status assign_args(syntax_state *state, label_table *_label_table, keyword *keyword_table);
status process_data_command(syntax_state *state, label_table *_label_table);
status process_string_command(syntax_state *state, label_table *_label_table);
status assign_addressing_method(syntax_state *state, char *argument, label_table *_label_table, keyword *keyword_table);
status validate_data_members(syntax_state *state);
status assign_addresses(inst_table *_inst_table, label_table *_label_table, keyword *_keyword_table);

#endif 
