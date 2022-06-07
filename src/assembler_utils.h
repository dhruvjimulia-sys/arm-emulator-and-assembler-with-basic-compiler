#include <stdint.h>

typedef struct {
	char *opcode;
	char **operand;
	uint32_t num_operands;
} TokenizedInstruction;
