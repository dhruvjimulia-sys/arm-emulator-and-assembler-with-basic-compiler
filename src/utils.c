#include <stdint.h>
#include <stdlib.h>
#include "type_definitions.h"
#include "utils.h"

#define OPCODE_MASK 0x1e00000
#define I_BIT_MASK 1<<25
#define RN_MASK 0xf0000
#define RD_MASK 0xf000
#define OP2_OFFSET_MASK 0xfff
#define RM_MULT_MASK 0b11
#define RS_MULT_MASK 0xf00
#define RN_MULT_MASK 0xf000
#define RD_MULT_MASK 0xf0000
#define BRANCH_OFFSET_MASK 0xffffff
#define SET_COND_CODES 1<<20
#define RESULT_31_BIT 1<<31
#define V_BIT_MASK 1<<28
#define Z_BIT_MASK 1<<30
#define C_BIT_MASK 1<<29

//get instruction type
instr_type get_instr_type(instr) {
	uint32_t type_bits = (instr >> 26) & 0b11;

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

//get opcode from instr
unsigned int get_opcode(uint32_t instr) {
	return (OPCODE_MASK & instr);
}

//get the value of I bit of the instruction
unsigned int immediate(uint32_t instr) {
	return (I_BIT_MASK & instr);
}

//get rn index (of operand 1) from instr
unsigned int get_rn_index(uint32_t instr) {
	return (RN_MASK & instr);
}

//get rd index from instr
unsigned int get_rd_index(uint32_t instr) {
        return (RD_MASK & instr);
}

//get operand 2 (data processing instruction) or offset (single data transfer instruction)
unsigned int operand2_or_offset(uint32_t instr) {
	return (OP2_OFFSET_MASK & instr);
}

//rotates an integer right by the specified rotation value
uint32_t rotate_right(uint32_t n, unsigned int rot_val) {
	const unsigned int mask = (8*sizeof(n) - 1);
	rot_val &= mask;
	return (n >> rot_val) | (n << ((-rot_val)&mask));
}

//get rs index from multiply instruction
unsigned int get_rs_mult(uint32_t instr) {
	return (RS_MULT_MASK & instr);
}

//get rn index from multiply instruction
unsigned int get_rn_mult(uint32_t instr) {
	return (RN_MULT_MASK & instr);
}

//get rm index from multiply instruction
unsigned int get_rm_mult(uint32_t instr) {
	return (RM_MULT_MASK & instr);
}

//get rd index from multiply instruction
unsigned int get_rd_mult(uint32_t instr) {
	return (RD_MULT_MASK & instr);
}

//get offset from branch instruction
unsigned int get_branch_offset(uint32_t instr) {
	return (BRANCH_OFFSET_MASK & instr);
}

//get set condition codes bit
unsigned int set_cond_codes(uint32_t instr) {
	return (SET_COND_CODES & instr);
}

//set N condition code
void set_n(uint32_t *cpsr, int32_t result) {
	*cpsr |= (result & RESULT_31_BIT);
}

//set Z condition code
void set_z(uint32_t *cpsr, int32_t result) {
	if (result == 0) {
		*cpsr |= Z_BIT_MASK;
	}
}

//set C condition code
void set_c(uint32_t *cpsr, int32_t set_value) {
	if (set_value) {
		*cpsr |= C_BIT_MASK;
	}
}

//set V condition code
void set_v(uint32_t *cpsr, int32_t result) {
	*cpsr |= V_BIT_MASK;
}
