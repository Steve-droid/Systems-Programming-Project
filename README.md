# Assembler Project

This project implements two-pass assembler developed as a final project for a systems programming course. This assembler is designed to translate assembly language(A pseudo-assembly language that was provided by the course staff) code into machine code, providing a toolset for assembly language processing.

## Features

- **Two-Pass Assembly**: Implements a two-pass assembly process for accurate symbol resolution and code generation.
- **Macro Support**: Includes a pre-assembler for macro expansion.
- **Error Handling**: Error detection and reporting for various syntax and semantic errors.
- **Multiple File Processing**: Capable of processing multiple input files in a single run.
- **Output Generation**: Produces object files (.ob), entry files (.ent), and external files (.ext) as needed.

## Project Structure

The project is organized into several key components:

- `pre_assembler.c/.h`: Handles macro expansion and initial file processing.
- `lexer.c/.h`: Performs lexical analysis of the assembly code.
- `parser.c/.h`: Parses the tokenized code and generates the machine code.
- `symbols.c/.h`: Manages symbol tables for labels and other identifiers.
- `data_structs.c/.h`: Defines and implements core data structures used throughout the assembler.
- `file_util.c/.h`: Provides utility functions for file handling.
- `text_util.c/.h`: Offers text processing utilities.
- `asm_error.c/.h`: Handles error reporting and management.

## Building and Running

To compile the project, use a C compiler such as GCC:

```bash
gcc *.c -o assembler
```

To run the assembler:

```bash
./assembler <input_file1.as> [input_file2.as ...]
```

Replace `<input_file1.as>` with your assembly source file. You can provide multiple input files.

## Input and Output

- Input: Assembly language files with `.as` extension.
- Output:
  - `.ob`: Object file containing the machine code.
  - `.ent`: Entry file listing entry points.
  - `.ext`: External file listing external references.

## Error Handling

The assembler provides detailed error messages for various issues including syntax errors, undefined symbols, and invalid operations. Error messages are displayed with the corresponding line number and description.

## Contributing

This project was developed as part of a university course. While it's not actively maintained, suggestions and improvements are welcome. Please open an issue to discuss potential changes.

