#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "type_definitions.h"
#include "utils.h"

#define Z_FLAG 0b100
#define V_FLAG 0b1
#define C_FLAG 0b10
#define MULT_BITS 0x00000090

//sign extension
int32_t sign_extend_26(int32_t extendable) {
	static const int SIGN_EXTEND = 4227858432U;
	return extendable + SIGN_EXTEND;
}

//reverses loaded bytes
uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

//extract bit and return boolean
bool extract_bit(uint8_t position, uint32_t* instruction) {
	return create_mask(position, position, instruction) == 1;
}

//create a bit mask for 32b instruction
uint32_t create_mask(uint8_t start, uint8_t finish, uint32_t* instruction) {
        uint32_t r;
        r = ((1 << (finish - start)) - 1) << start;
        return (r & *(instruction)) >> start;
}

//clear the contents of array (to all zero)
void clear_array(uint8_t* arr, uint64_t length) {
	for (int i = 0; i < length; i++) {
		arr[i] = 0;
	}
}

//checks if array contains all zeroes
bool is_all_zero(uint8_t* arr, uint64_t length) {
	for (int i = 0; i < length; i++) {
		if (arr[i] != 0) {
			return false;
		}
	}
	return true;
}

//get instruction type
instr_type get_instr_type(uint32_t *instr) {
	uint32_t type_bits = create_mask(26, 27, instr);

	switch(type_bits) {
		case 0:
			if (create_mask(4, 7, instr) == MULT_BITS) {
				return MULTIPLY;
			} else {
				return DATA_PROCESS;
			}
		case 1: 
			return TRANSFER;
		case 2:
			return BRANCH;
		default:
			exit(EXIT_FAILURE);
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
