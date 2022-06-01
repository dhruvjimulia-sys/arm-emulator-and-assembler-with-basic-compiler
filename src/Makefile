CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
BUILD = assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

emulate:	emulate.o type_definitions.h utils.o instructions.o
			$(CC) emulate.o type_definitions.h utils.o instructions.o -o emulate
emulate.o:	emulate.c emulate.h type_definitions.h utils.h instructions.h
			$(CC) $(CFLAGS) -c emulate.c
instructions.o:	instructions.c type_definitions.h utils.h instructions.h
			$(CC) $(CFLAGS) -c instructions.c
utils.o:	utils.c type_definitions.h utils.h
			$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
