/**
 * @file parser.h
 * @brief Header file for the syntax analyzer (parser) component of the assembly code translator.
 *
 * The parser (syntax analyzer) is a crucial component in the process of translating
 * the made-up assembly-like code into machine code or another target format.
 * This file contains the declarations and definitions for the parser component.
 *
 * The primary role of the parser is to:
 * - Read the stream of tokens generated by the lexer.
 * - Analyze the token sequence according to the grammatical rules of the assembly-like language.
 * - Construct a parse tree or abstract syntax tree (AST) that represents the hierarchical structure of the source code.
 * - Ensure that the token sequence forms a syntactically valid program.
 * - Report syntactic errors, such as unexpected tokens or missing operators.
 *
 * The parser transforms the flat sequence of tokens into a structured representation
 * that reflects the logical and syntactic organization of the code, facilitating further stages of translation.
 */


#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "instruction.h"


status parse(inst_table *_inst_table, label_table *_label_table, keyword *keyword_table);



#endif