CC = gcc $(SAN)
CFLAGS = -Wall -g -pedantic --std=c99
BUILD = compiler

all:	compiler

clean:
	rm -rf *.o $(BUILD)

compiler:	compiler.o symbol_table.o

compiler.o:	compiler.c symbol_table.c

symbol_table.o: symbol_table.c
