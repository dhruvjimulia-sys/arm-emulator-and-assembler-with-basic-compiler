#ifndef EMUL_ENUMS_H
#define EMUL_ENUMS_H

typedef enum InstructionType {
	DATA_PROCESS,
	MULTIPLY,
	TRANSFER,
	BRANCH,
	NO_OPERATION = 0xffffffff
} instr_type;

#endif
