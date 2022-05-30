#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "utils.h"
#include "emulator.c"

#define IMM_OP2 0xff
#define ROT_VAL_IMM_OP2 0xf00
#define REG_OP2_RM_MASK 0xf
#define BIT_4_MASK 0x10

void execute_data_processing_instruction(struct Processor processor, uint32_t instr) {

	int opcode = get_opcode(instr);
	int set_cpsr = set_cond_codes(instr);
	int rd_index = get_rd_index(instr);
	int rn_index = get_rn_index(instr);
	int rm_val = operand2_or_offset(instr);
	int32_t op2;
	int32_t result;
	uint32_t dest = processor->registers[rd_index];
	uint32_t rn_val = processor->registers[rn_index];	
	uint32_t cpsr = processor->registers[CPSR];

	if (immediate(instr)) {
		/* operand2 is an immediate constant */
		rm_val = instr & IMM_OP2;
		//get rotation value
		uint32_t rot_val = 2 * ((instr & ROT_VAL_IMM_OP2) >> 8);
		op2 = rotate_right(rm_val, rot_val);
	} else {
		/* operand is shifted register */
		op2 = shift_rm_register(rm_val, processor, set_cpsr);
	}

	uint32_t op2_unsigned = op2;

	switch(opcode) {
		case 0: /* and, rn AND Operand2 */
			dest = rn_val & op2;
			result = dest;
			break;
		case 1: /* eor, rn EOR Operand2 */
			dest = rn_val ^ op2;
			result = dest;
			break;
		case 2: /* sub, rn - operand2 */
			dest = rn_val -op2;
			result = dest;

			if (set_cpsr) {
				//set C flag if subtraction produced a borrow
				set_c(cpsr,rn_val < op2_unsigned);
			}
			break;
		case 3: /* rsub, reverse sub, operand2 - rn */
			dest = op2 - (rn_val);
			result = dest;
			if (set_cpsr) {
				//set C flag if subtraction produced a borrow
				set_c(cpsr,rn_val > op2);
			}
			break;
		case 4: /* add, rn + operan2 */
			dest = rn_val + op2;
			result = dest;

			if(set_cpsr) {
				//set C flag if unsigned overflow occurs
  				set_c(cpsr,(rn_val + op2_unsigned) < rn_val);
			}
			break;
		case 8: /* tst, and (but result is not written) */
			result = rn_val & op2;
			break;
		case 9: /* teq, eor (but result is not written) */
			result = rn_val ^ op2;
			break;
		case 10: /* cmp, sub (but result is not written) */
			result = rn_val - op2;

			if (set_cpsr) {
				//set C flag if subtraction produced a borrow
				set_c(cpsr,rn_val > op2_unsigned);
			}
			break;
		case 12: /* orr, rn OR operand2 */
			result = rn_val | op2;
			break;
		case 13: /* mov, operand2 is moved, Rn is ignored */
			dest = op2;
			result = dest;
			break;
		default:
			printf("Invalid opcode.\n");
			return EXIT_FAILURE;
	}

	if (set_cpsr) {	
		set_z(cpsr, result);
		set_n(cpsr, result);
	}
}

int shift_rm_register(int op2, struct Processor processor, int set_cpsr) {
	unsigned int rm = op2 & REG_OP2_RM_MASK;
	//for the optional part where shift is specified by rs register
	unsigned int bit4 = op2 & BIT_4_MASK; 
	unsigned int shift_type = ((op2 & 0x60) >> 5);
	unsigned int shift_const_amount;
	uint32_t *cpsr = processor->registers[CPSR];

	if (bit4) {
		//shift amount is specified by rs
		int rs = ((op2 >> 8) & 0xf);
		shift_const_amount = processor->registers[rs] & 0xff;
	} else {
		//shift amount is a constant amount
		shift_const_amount = ((op2 >> 7) & 0x1f);
	}

	return shift(processor->registers[rm], shift_type, shift_const_amount,cpsr, set_cpsr);
}

void execute_branch_instruction(uint32_t instr) {
	unsigned int offset = branch_offset(instr);
	int32_t dest;

	dest = offset << 2;
	//processor->registers[PC] = dest + 8;
}

int32_t shift(uint32_t n, unsigned int shift_type, unsigned int shift_amount, 
			uint32_t set_cpsr, uint32_t *cpsr) {
	int32_t cout = 0;
	int result = 0;
	
	switch(shift_type) {
		case 0: /* lsl */
			cout = n & (1 << (32 - shift_amount));
			result = n << shift_amount;
			break;
		case 1: /* lsr */
			cout = n & (1 << (shift_amount - 1));
			result = n >> shift_amount;
			break;
		case 2: /* asr */
			cout = n & (1 << (shift_amount - 1));
			result = ((int) n) >> shift_amount;
			break;
		case 3: /* ror */
			cout = n & (1 << (shift_amount - 1));
			result = rotate_right(n, shift_amount);
			break;
		default:
			printf("Invalid shift type.\n");
			return EXIT_FAILURE;
	}

	//set C condition code to carry out from the shifter
	if (set_cpsr) {
		set_c(cpsr, cout);
	}

	return result;
}

void execute_multiply_instruction(struct Processor processor, uint32_t instr) {
	uint32_t rm_val = processor->registers[get_rm_mult(instr)];
	uint32_t rs_val = processor->registers[get_rs_mult(instr)];
	uint32_t cpsr = processor->registers[CPSR];
	int32_t set_cpsr = set_cond_codes(instr);
	
	uint64_t product = rm_val * rs_val;

	unsigned int acc_cond_bit = (instr >> 21) & 0b1;
	
	/* multiply and accumulate instruction */
	if (acc_cond_bit) {
		product += processor->registers[get_rn_mult(instr)];
	}
	
	//truncate product (multiply instruction result) to 32 bits
	uint32_t product32b = product;
	processor->registers[get_rd_mult(instr)] = product32b;

	//set condition codes
	if (set_cpsr) {	
		set_z(cpsr, product32b);
		set_n(cpsr, product32b);
	}
}

void execute_single_data_transfer(struct Processor processor, uint32_t instr) {

	int opcode = get_opcode(instr);
	int set_cpsr = set_cond_codes(instr);
	//should we declare functions for this too to avoid magic numbers?
	int p_bit = (instr >> 24) & 0b1;
	int u_bit = (instr >> 23) & 0b1;
	int l_bit = (instr >> 20) & 0b1;
	//implement function to extract op2 from data processing / offset from data transfer
	unsigned int offset = (instr & 0xfff);
	int rd_index = get_rd_index(instr);
	int rn_index = get_rn_index(instr);
	uint32_t address = 0;
	uint32_t *dest = processor->registers[rd_index];
	uint32_t *base_reg = processor->registers[rn_index];


	if (immediate(instr)) {
		//offset interpreted as a shifted register
		offset = shift_rm_register(offset, processor, set_cpsr);
	}

	if (p_bit) {
		//if i got it right, we do not change base_reg value if pre-indexing
		//then this bit of code is not necessary
		/*
		if (u_bit) {
			base_reg += offset;
		} else {
			base_reg -= offset;
		}
		*/
	} 

	if (p_bit == 0) {
		//post-indexing
		assert (rd_index != rn_index);

		if (l_bit) {
			//ldr, load word from memory
			dest = *((uint32_t*) base_reg);
		} else {
			//str, store word in memory
			*((uint32_t*) base_reg) = dest;
		}

		if (u_bit) {
			base_reg += offset;
		} else {
			base_reg -= offset;
		}
	}
}
