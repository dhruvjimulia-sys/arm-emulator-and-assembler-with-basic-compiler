#include <stdint.h>

#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

typedef struct {
	Operation *opcode;
	char **operand;
	uint32_t num_operands;
} TokenizedInstruction;

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

//rotate value right given amount of times
static uint32_t rotate_right(uint32_t n, unsigned int shift_amount);

//set necessary bits to 1 in a 32bit value
void set_bits_to(uint32_t *instr, uint32_t input, unsigned int starting_at);

#endif
