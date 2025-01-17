###################################
#
# Makefile
#
# "Saper"
# Aleksander Slepowronski 2025
#
###################################


# -- VARIABLES --

# All source files:
SRC = $(wildcard src/*.c)

# -- BUILD --

# Basic build:
main:
	gcc $(SRC) -o bin/saper.out -lm -O2 -std=c11 -DNDEBUG

# Debug build:
debug:
	gcc $(SRC) -o bin/dsaper.out -lm -O0 -std=c11

# Basic build (Windows):
winb:
	gcc $(SRC) -o saper.exe -lm -O2 -std=c11 -DNDEBUG



