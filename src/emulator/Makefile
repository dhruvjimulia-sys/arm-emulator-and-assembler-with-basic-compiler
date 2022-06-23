CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic 
BUILD = emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

emulate:	emulate.o emulator_utils.o instructions.o
emulate.o:	type_definitions.h emulator_utils.h instructions.h emulate.h
instructions.o:	type_definitions.h emulator_utils.h instructions.h
emulator_utils.o:	type_definitions.h emulator_utils.h

clean:
	rm -f $(wildcard *.o)
	rm -f emulate