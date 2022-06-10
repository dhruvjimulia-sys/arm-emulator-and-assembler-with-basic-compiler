#include <stdint.h>

#ifndef TYPE_DEFINITIONS_H
#define TYPE_DEFINITIONS_H

#define MEM_SIZE 65536
#define REGISTERS 17
#define GPIO_SIZE 20

//processor structure storing state of processor
struct Processor {
	uint8_t memory[MEM_SIZE];
	uint32_t registers[REGISTERS];
};

typedef enum InstructionType {
	DATA_PROCESS,
	MULTIPLY,
	TRANSFER,
	BRANCH
} instr_type;

enum Flags {
	eq = 0,
	ne = 1,
	ge = 10,
	lt = 11,
	gt = 12,
	le = 13,
	al = 14,
};

#endif
