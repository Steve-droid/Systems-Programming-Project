# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c90 -g

# Include directories
INCLUDES = -I.

# Source files
SRCS = decode_to_int_array.c \
       decode_to_string_array.c \
       decoding.c \
       file_util.c \
       help_table.c \
       label_table.c \
       macro_table.c \
       macro.c \
       main.c \
       pre_assembler.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = Test

# Default target
all: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJS)
	@echo "Linking object files to create executable..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXEC) $(OBJS)
	@echo "Build complete."

# Compile source files to object files
decode_to_int_array.o: decode_to_int_array.c decode_to_int_array.h
	@echo "Compiling decode_to_int_array.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c decode_to_int_array.c -o decode_to_int_array.o

decode_to_string_array.o: decode_to_string_array.c decode_to_string_array.h
	@echo "Compiling decode_to_string_array.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c decode_to_string_array.c -o decode_to_string_array.o

decoding.o: decoding.c decoding.h
	@echo "Compiling decoding.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c decoding.c -o decoding.o

file_util.o: file_util.c file_util.h
	@echo "Compiling file_util.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c file_util.c -o file_util.o

help_table.o: help_table.c help_table.h
	@echo "Compiling help_table.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c help_table.c -o help_table.o

label_table.o: label_table.c label_table.h
	@echo "Compiling label_table.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c label_table.c -o label_table.o

macro_table.o: macro_table.c macro_table.h
	@echo "Compiling macro_table.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c macro_table.c -o macro_table.o

macro.o: macro.c macro.h
	@echo "Compiling macro.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c macro.c -o macro.o

main.o: main.c
	@echo "Compiling main.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c -o main.o

pre_assembler.o: pre_assembler.c pre_assembler.h
	@echo "Compiling pre_assembler.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c pre_assembler.c -o pre_assembler.o

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)
	@echo "Clean complete."

# Phony targets
.PHONY: all clean
