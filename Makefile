#
# 	CAB202
#	Generic Makefile for compiling with floating point printf
#
#	B.Talbot, September 2015
#	Queensland University of Technology
#

# Modify these
SRC=main
CAB202_LIB_DIR=lib/cab202_teensy

# The rest should be all good as is
FLAGS=-mmcu=atmega32u4 -Os -DF_CPU=8000000UL -std=gnu99 -Wall -Werror
LIBS=-Wl,-u,vfprintf -lprintf_flt -lcab202_teensy -lm

# Default 'recipe'
all:
	avr-gcc src/$(SRC).c $(FLAGS) -I$(CAB202_LIB_DIR) -L$(CAB202_LIB_DIR) $(LIBS) -o out/$(SRC).o
	avr-objcopy -O ihex out/$(SRC).o out/$(SRC).hex

# Cleaning  (be wary of this in directories with lots of executables...)
clean:
	rm out/*.o
	rm out/*.hex
