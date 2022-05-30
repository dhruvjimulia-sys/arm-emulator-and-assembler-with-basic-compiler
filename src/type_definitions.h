#ifndef EMUL_ENUMS_H
#define EMUL_ENUMS_H

//do we keep this, or go with just testing the instruction with a mask each time?
//with enum, make a switch statement and might be cleaner
typedef enum InstructionType {
	DATA_PROCESS,
	MULTIPLY,
	TRANSFER,
	BRANCH,
	NO_OPERATION = 0xffffffff
} instr_type;

#endif
