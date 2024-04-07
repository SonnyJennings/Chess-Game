CMAKE_MINIMUM_REQUIRED(VERSION 3.0) # set a minimum required version of cmake

project(UltimateChess VERSION 1.0 LANGUAGES C) # project name version and languages

set(CMAKE_C_COMPILER /usr/bin/gcc) # set the compiler

set(CMAKE_C_FLAGS "-Wall -Wextra -std=c99 -pthread") # flags that are always used
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -Og") # debug flags
set(CMAKE_C_FLAGS_RELEASE "-Ofast -march=native -DNDEBUG") # release flags

set(EXECUTABLE_OUTPUT_PATH ../bin)  # executables go to the bin directory

include_directories( ../include) # include the headers in the include folder

file(GLOB SOURCES ../src/*.c)  # use all of the .c files in the src folder as the sources

add_executable(UltimateChess ${SOURCES})  # add an executable compiling the sources

