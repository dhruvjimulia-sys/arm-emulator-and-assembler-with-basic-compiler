#ifndef EMUL_ENUMS_H
#define EMUL_ENUMS_H

#define MEM_SIZE 65536
#define REGISTERS 17

//processor structure storing state of processor
struct Processor {
        uint8_t memory[MEM_SIZE];
        uint32_t registers[REGISTERS];
};

enum InstructionType {
	DATA_PROCESS,
	MULTIPLY,
	TRANSFER,
	BRANCH,
	NO_OPERATION = 0xffffffff
};

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
