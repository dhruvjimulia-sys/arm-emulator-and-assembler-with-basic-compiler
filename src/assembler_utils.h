#include <stdint.h>

typedef struct {
	char *opcode;
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

