#!/bin/bash

#######################################
## RawDev project makefile generator ##
## 2023, Petr Krajnik                ##
## usage: tools/makemake.sh           ##
#######################################

# Check if we in the right directory
if [ ! -d src/ ]; then
	echo 'Source codes directory not found!'
	echo 'Working directory must be project root directory.'
	echo 'Exit with code 1'
	exit 1
fi

# Prolog header
echo -e "\
# Makefile for build 'rawdev' in Linux/MacOs/BSD
# Generated by the 'makemake.sh' script
# Dependencies generated with 'g++ -MM'
# `date`\n
PROGRAM = rawdev
BUILD_DIR = build/maked
OBJ_DIR = \$(BUILD_DIR)/obj

# Use clang++ for MacOS and BSD (for Linux use GCC)
# CC = clang++

# Use GCC for Linux
CC = g++

# Compiler flags (O3 make warnings with GCC)
CCFLAGS = -std=c++14 -Wall -pedantic -O2 -fopenmp -I src/
"

# Find all sources to compile. Sorted.
sources=`find src/ -type f -name *.cpp | sort`

# Generate object list
echo -n 'OBJECTS ='
for f in $sources; do
	echo -e ' \'
	echo -en "\t\$(OBJ_DIR)/$(basename $f .cpp).o"
done

# Print basic targets
echo -e "\n
all: prepare \$(BUILD_DIR)/\$(PROGRAM)
\t@echo 'DONE!'

prepare:
\t@mkdir -p \$(BUILD_DIR)/obj

clear:
\trm -rf \$(BUILD_DIR)

.PHONY: all prepare clear

#########
"

# Main target
echo -e "\$(BUILD_DIR)/\$(PROGRAM): \$(OBJECTS)
\t\$(CC) \$(CCFLAGS) -o \$@ \$?
"

# File rules with dependencies
for f in $sources; do
	echo -n '$(OBJ_DIR)/'
	gcc -MM -I src/ $f # Dependency generation
	echo -e "\t\$(CC) \$(CCFLAGS) -c -o \$@ $f\n"
done
