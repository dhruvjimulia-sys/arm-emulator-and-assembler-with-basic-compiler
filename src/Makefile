CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic 
BUILD = assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o symbol_table.o tokenizer.o assemble_instructions.o
assemble.o: assemble.h symbol_table.h tokenizer.h assemble_instructions.h assembler_type_definitions.h
assemble_instructions.o: assemble_instructions.h
tokenizer.o: tokenizer.h assembler_type_definitions.h
symbol_table.o: symbol_table.h

emulate:	emulate.o emulator_utils.o instructions.o
emulate.o:	type_definitions.h emulator_utils.h instructions.h emulate.h
instructions.o:	type_definitions.h emulator_utils.h instructions.h
emulator_utils.o:	type_definitions.h emulator_utils.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
