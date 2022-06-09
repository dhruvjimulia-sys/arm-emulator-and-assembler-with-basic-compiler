#include <stdint.h>

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


typedef struct {
	Operation opcode;
	char **operand;
	uint32_t num_operands;
} TokenizedInstruction;

//rotate value right given amount of times
static uint32_t rotate_right(uint32_t n, unsigned int shift_amount) {
  if ((shift_amount &= sizeof(n) * 8 - 1) == 0)
    return n;
  return (n << shift_amount) | (n >> (sizeof(n) * 8 - shift_amount));
}
