# Compiler
CC = gcc

# Compiler flags
CFLAGS =   -g -Wall -ansi -pedantic 

# Include directories
INCLUDES = -I./header_files

# Source directory
SRCDIR = source_files

# Object directory
OBJDIR = object_files

# Output directory
OUTDIR = output_files

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c)

# Object files
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)


# Executable name
EXEC = assembler

# Default target
all: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJS)
	@echo "Linking object files to create executable..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^
	@echo "Build complete."

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create the object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)


# Clean up
clean:
	rm -f $(OBJDIR)/*.o $(EXEC) *.am 
	rm -f *.ent *.ext *.ob *.binary
	@echo "Clean complete."

reset:
	rm -f *.ent *.ext *.ob  *.am
	@echo "Reset complete"	

# Phony targets
.PHONY: all clean
