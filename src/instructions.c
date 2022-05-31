#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

int32_t shift(uint32_t n, unsigned int shift_type, unsigned int shift_amount, 
			uint32_t set_cpsr, uint32_t *cpsr_reg) {
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
		set_c(cpsr_reg, cout);
	}

	return result;
}

int shift_rm_register(uint32_t *instr, struct Processor processor, int set_cpsr) {
	unsigned int rm = create_mask(0, 3, instr);
	//for the optional part where shift is specified by rs register
	bool bit4 = extract_bit(4, instr); 
	unsigned int shift_type = create_mask(5, 6, instr);
	unsigned int shift_amount;
	uint32_t *cpsr_reg = &processor.registers[CPSR_REGISTER];

	if (bit4) {
		//shift amount is specified by rs
		int rs = create_mask(8, 11, instr);
		shift_amount = processor.registers[rs] & 0xff;
	} else {
		//shift amount is a constant amount
		shift_amount = create_mask(7, 11, instr);
	}

	return shift(processor.registers[rm], shift_type, shift_amount, set_cpsr, cpsr_reg);
}

void execute_data_processing_instruction(struct Processor processor, uint32_t *instr) {

	int opcode = create_mask(21, 24, instr);
	bool set_cpsr = extract_bit(20, instr);
	uint32_t rm_val = create_mask(0, 11, instr);
	int32_t op2;
	int32_t result;
	uint32_t *dest = &processor.registers[create_mask(12, 15, instr)];
	uint32_t rn_val = processor.registers[create_mask(16, 19, instr)];	
	uint32_t *cpsr_reg = &processor.registers[CPSR_REGISTER];

	if (extract_bit(25, instr)) {
		/* operand2 is an immediate constant */
		rm_val = create_mask(0, 7, instr);
		//get rotation value
		uint32_t rot_val = 2 * create_mask(8, 11, instr);
		op2 = rotate_right(rm_val, rot_val);
	} else {
		/* operand is shifted register */
		op2 = shift_rm_register(instr, processor, set_cpsr);
	}

	uint32_t op2_unsigned = op2;

	switch(opcode) {
		case 0: /* and, rn AND Operand2 */
			*dest = rn_val & op2;
			result = *dest;
			break;
		case 1: /* eor, rn EOR Operand2 */
			*dest = rn_val ^ op2;
			result = *dest;
			break;
		case 2: /* sub, rn - operand2 */
			*dest = rn_val -op2;
			result = *dest;

			if (set_cpsr) {
				//set C flag if subtraction produced a borrow
				set_c(cpsr_reg,rn_val < op2_unsigned);
			}
			break;
		case 3: /* rsub, reverse sub, operand2 - rn */
			*dest = op2 - (rn_val);
			result = *dest;
			if (set_cpsr) {
				//set C flag if subtraction produced a borrow
				set_c(cpsr_reg,rn_val > op2);
			}
			break;
		case 4: /* add, rn + operan2 */
			*dest = rn_val + op2;
			result = *dest;

			if(set_cpsr) {
				//set C flag if unsigned overflow occurs
  				set_c(cpsr_reg,(rn_val + op2_unsigned) < rn_val);
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
				set_c(cpsr_reg,rn_val > op2_unsigned);
			}
			break;
		case 12: /* orr, rn OR operand2 */
			result = rn_val | op2;
			break;
		case 13: /* mov, operand2 is moved, Rn is ignored */
			*dest = op2;
			result = *dest;
			break;
		default:
			printf("Invalid opcode.\n");
	}

	if (set_cpsr) {	
		set_z(cpsr_reg, result);
		set_n(cpsr_reg, result);
	}
}

void execute_branch_instruction(uint32_t *instr) {
	unsigned int offset = create_mask(0, 23, instr);
	int32_t dest;

	dest = offset << 2;
	processor.registers[PC_REGISTER] = dest + 8;
}

void execute_multiply_instruction(struct Processor processor, uint32_t *instr) {
	uint32_t rm_val = processor.registers[create_mask(0, 3, instr)];
	uint32_t rs_val = processor.registers[create_mask(8, 11, instr)];
	uint32_t *cpsr_reg = &processor.registers[CPSR_REGISTER];
	bool set_cpsr = extract_bit(20, instr);
	
	uint64_t product = rm_val * rs_val;

	bool acc_cond_bit = extract_bit(21, instr);
	
	/* multiply and accumulate instruction */
	if (acc_cond_bit) {
		product += processor.registers[create_mask(12, 15, instr)];
	}
	
	//truncate product (multiply instruction result) to 32 bits
	uint32_t product32b = product;
	processor.registers[create_mask(16, 19, instr)] = product32b;

	//set condition codes
	if (set_cpsr) {
		set_z(cpsr_reg, product32b);
		set_n(cpsr_reg, product32b);
	}
}

void execute_single_data_transfer(struct Processor processor, uint32_t *instr) {
	bool p_bit = extract_bit(24, instr);
	bool u_bit = extract_bit(23, instr);
	bool l_bit = extract_bit(20, instr);
	unsigned int offset = create_mask(0, 11, instr);
	int rd_index = create_mask(12, 15, instr);
	int rn_index = create_mask(16, 19, instr);
	uint32_t *dest = &processor.registers[rd_index];
	uint32_t *base_reg = &processor.registers[rn_index];


	if (extract_bit(25, instr)) {
		// offset interpreted as a shifted register
		offset = shift_rm_register(instr, processor, 0);
	}

	if (l_bit) {
		//ldr, load word from memory
		*dest = *((uint32_t*) base_reg);
	} else {
		//str, store word in memory
		*((uint32_t*) base_reg) = *dest;
	}

	if (p_bit == 0) {
		//post-indexing
		assert (rd_index != rn_index);

		if (u_bit) {
			base_reg += offset;
		} else {
			base_reg -= offset;
		}
	}
}
