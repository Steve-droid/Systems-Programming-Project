cmake_minimum_required(VERSION 3.10)
project(MMN14 C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -g")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

include_directories(header_files)

file(GLOB SOURCES "source_files/*.c")

add_executable(assembler ${SOURCES})
