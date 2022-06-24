CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic 
BUILD = assemble emulate compiler

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

emulate:
	cd emulator; make clean all; cd ..
assemble:
	cd assembler; make clean all; cd ..
compiler:
	cd compile; make clean all; cd ..
clean:
	cd emulator; make clean; cd ../assembler; make clean; cd ../compile; make clean; cd ..
