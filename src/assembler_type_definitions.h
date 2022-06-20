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
	PRINT,
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

typedef enum {
	LSL_SHIFT,
	LSR_SHIFT,
	ASR_SHIFT,
	ROR_SHIFT
} Shift;

typedef struct {
	Operation opcode;
	char **operand;
	uint32_t num_operands;
} TokenizedInstruction;

#endif
