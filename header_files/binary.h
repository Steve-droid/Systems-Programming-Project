#ifndef BINARY_H
#define BINARY_H

#include "label.h"
#include "keyword.h"

#define MAX_BINARY_COMMAND_LENGTH 15

typedef struct {
    int binary[MAX_BINARY_COMMAND_LENGTH];
    int length;
} binary_command;

typedef struct {
    binary_command **commands;
    int size;
} binary_table;

/**
 * @brief Creates a new binary command.
 *
 * @param length The length of the binary command.
 * @return A pointer to the newly created binary command.
 */
binary_command *create_binary_command(int length);

/**
 * @brief Creates a new binary table.
 *
 * @return A pointer to the newly created binary table.
 */
binary_table *create_binary_table();

/**
 * @brief Adds a command to the binary table.
 *
 * @param table The binary table.
 * @param cmd The binary command to add.
 */
void add_command_to_table(binary_table *table, binary_command *cmd);

/**
 * @brief Frees the memory allocated for the binary table.
 *
 * @param table The binary table to free.
 */
void free_binary_table(binary_table *table);

/**
 * @brief Prints the binary table.
 *
 * @param table The binary table to print.
 */
void print_binary_table(binary_table *table);



#endif /* BINARY_H */

