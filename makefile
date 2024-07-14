# Makefile

CC = gcc
CFLAGS = -g -Wall -std=c99
TARGET = pre_assemble

SRC = pre_asm_init.c pre_assembler.c file_util.c macro.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJ) $(TARGET) *.am
	