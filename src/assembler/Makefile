CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic 
BUILD = assemble

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o symbol_table.o tokenizer.o assemble_instructions.o
assemble.o: assemble.h symbol_table.h tokenizer.h assemble_instructions.h assembler_type_definitions.h
assemble_instructions.o: assemble_instructions.h
tokenizer.o: tokenizer.h assembler_type_definitions.h
symbol_table.o: symbol_table.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble