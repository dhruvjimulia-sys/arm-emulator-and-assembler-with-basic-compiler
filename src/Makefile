CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
BUILD = assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

emulate:	emulate.o utils.o instructions.o
emulate.o:	type_definitions.h utils.h instructions.h emulate.h
instructions.o:	type_definitions.h utils.h instructions.h
utils.o:	type_definitions.h utils.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
