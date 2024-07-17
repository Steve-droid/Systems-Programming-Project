#include <stdio.h>
#include <stdlib.h>
#include "binary.h"

binary_command *create_binary_command(int length) {
    binary_command *cmd = (binary_command *)malloc(sizeof(binary_command));
    if (cmd == NULL) {
        printf("ERROR- ALLOCATION FAILED");
        return NULL;
    }
    cmd->length = length;
    return cmd;
}

binary_table *create_binary_table() {
    binary_table *table = (binary_table *)malloc(sizeof(binary_table));
    if (table == NULL) {
        printf("ERROR- ALLOCATION FAILED");
        return NULL;
    }
    table->size = 0;
    table->commands = NULL;
    return table;
}

void add_command_to_table(binary_table *table, binary_command *cmd) {
    table->size++;
    table->commands = (binary_command **)realloc(table->commands, table->size * sizeof(binary_command *));
    if (table->commands == NULL) {
        printf("ERROR- ALLOCATION FAILED");
        return;
    }
    table->commands[table->size - 1] = cmd;
}

void free_binary_table(binary_table *table) {
    int i;
    for (i = 0; i < table->size; i++) {
        free(table->commands[i]);
    }
    free(table->commands);
    free(table);
}

void print_binary_table(binary_table *table) {
    int i, j;
    for (i = 0; i < table->size; i++) {
        for (j = 0; j < table->commands[i]->length; j++) {
            printf("%d ", table->commands[i]->binary[j]);
        }
        printf("\n");
    }
}
