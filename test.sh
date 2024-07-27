#!/bin/bash

# Check if the input file argument is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

input_file=$1

# Run valgrind with the specified options and input file, then pipe the output to ccze for color highlighting
./assembler "$input_file" | ccze -A