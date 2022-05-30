#include <stdint.h>
#include <stdlib.h>
#include "type_definitions.h"
#include "utils.h"
#include "emulator.c"

#define Z_FLAG 0b10
#define V_FLAG 0b1

//get instruction type
instr_type get_instr_type(uint32_t *instr) {
	uint32_t type_bits = create_mask(26, 27, instr);

	switch(type_bits) {
		case 0:
			//how would you classify between DATA_PROC and MULTIPLY
		case 1: 
			return TRANSFER;
			break;
		case 2:
			return BRANCH;
			break;
		default:
			printf("Instruction may not be supported.\n");
			return EXIT_FAILURE;
	}

}

//rotates an integer right by the specified rotation value
uint32_t rotate_right(uint32_t n, unsigned int rot_val) {
	const unsigned int mask = (8*sizeof(n) - 1);
	rot_val &= mask;
	return (n >> rot_val) | (n << ((-rot_val)&mask));
}

//set N condition code
void set_n(uint32_t *cpsr, int32_t result) {
	*cpsr |= (result & (1<<31));
}

//set Z condition code
void set_z(uint32_t *cpsr, int32_t result) {
	if (result == 0) {
		*cpsr |= Z_FLAG;
	}
}

//set C condition code
void set_c(uint32_t *cpsr, int32_t set) {
	if (set) {
		*cpsr |= C_FLAG;
	}
}

//set V condition code
void set_v(uint32_t *cpsr, int32_t result) {
	*cpsr |= V_FLAG;
}
