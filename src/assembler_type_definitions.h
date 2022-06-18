#ifndef __ASSEMBLER_TYPE_DEFINITIONS_H__
#define __ASSEMBLER_TYPE_DEFINITIONS_H__

#include <stdint.h>

typedef enum {
	ADD,
	SUB,
	RSB,
	ANDEQ,
	AND,
	EOR,
	ORR,
	LSL,
	MOV,
	TST,
	TEQ,
	CMP,
	MUL,
	MLA,
	LDR,
	STR,
	BEQ,
	BNE,
	BGE,
	BLT,
	BGT,
	BLE,
	B
} Operation;

typedef struct {
	Operation opcode;
	char **operand;
	uint32_t num_operands;
} TokenizedInstruction;

#endif
