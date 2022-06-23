CC = gcc $(SAN)
CFLAGS = -Wall -g -pedantic --std=c99
BUILD = tester

all:	tester

clean:
	rm -rf *.o $(BUILD)

tester:	tester.o

tester.o:	tester.c