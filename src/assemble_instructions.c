#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "assemble_instructions.h"
#include "emulator_utils.h"

/* bit fields start bit */
#define RD_START_BIT 12
#define RN_START_BIT 16
//#define PC_REGISTER 0xf
#define OPCODE_START_BIT 21
#define COND_FIELD_START_BIT 28
#define MULT_ONLY_START_BIT 4
#define OP2_OFFSET_REG 0
#define SHIFT_TO_IMM 8
#define SHIFT_BY_INT 7
#define SHIFT_BY_REG 8
#define SHIFT_TYPE_BITS 5
#define INSTR_FIELD 25
#define PRINT_INPUT_INSTR 0x7

/* positions of flag bits */
#define S_BIT 20
#define L_BIT 20
#define I_BIT 25
#define A_BIT 21
#define P_BIT 24
#define U_BIT 23
#define SHIFT_INT_OR_REG_BIT 4
/* for additional extension instructions imlemented */
#define T_BIT 22
#define R_BIT 21
#define SPECIAL_I_BIT 23

/* values based on instruction */
#define SET_BIT 0x1
#define ALWAYS_COND 0xe
#define TEST_INSTR_OPERAND_COUNT 2
#define COMPUTATION_INSTR_OPERAND_COUNT 3
#define BRANCH_INSTR 0x5
#define MULT_ONLY 0x9
#define SHIFT_INT_OR_REG_BIT 4
#define PIPELINE_EFFECT 0x8
#define TRANSFER_INSTR 0x2
#define MAX_REPRESENTABLE 0xff
#define MAX_ROTATION 16

/* shifter values */
#define LSL_SHIFT 0
#define LSR_SHIFT 1
#define ASR_SHIFT 2
#define ROR_SHIFT 3

void set_bits_to(uint32_t *instr, int32_t input, unsigned int starting_at) {
	*instr |= (input << starting_at);
}

uint32_t assemble_data_processing(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;
	uint32_t opcode;
	unsigned int shifted_reg = 0;

	//int i determines position of operand2 in the operands array (based on # of operands in the instructions)
	//preset to 1 (for tst, teq, cmp, mov)
	unsigned int i = 1;

	switch (token_instr->opcode) {
		case ANDEQ:
			//set_bits_to(assembled_instr, 0, 0);
			return 0;
		case AND:
			opcode = 0x0;
			break;
		case EOR:
			opcode = 0x1;
			break;
		case SUB:
			opcode = 0x2;
			break;
		case RSB:
			opcode = 0x3;
			break;
		case ADD:
			opcode = 0x4;
			break;
		case TST:
			opcode = 0x8;
			break;
		case TEQ:
			opcode = 0x9;
			break;
		case CMP:
			opcode = 0xa;
			break;
		case ORR:
			opcode = 0xc;
			break;
		case LSL:
		case MOV:
			opcode = 0xd;
			break;
		default:
			printf("Invalid opcode %x for data processing instruction.", token_instr->opcode);
			exit(EXIT_FAILURE);
	}

	set_bits_to(&assembled_instr, opcode, OPCODE_START_BIT);
	
	//set condition field to ALWAYS
        set_bits_to(&assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);

	//instructions that do not compute result, but set the CPSR flags
	if (opcode >= 0x8 && opcode <= 0xa) {
		if (token_instr->num_operands > TEST_INSTR_OPERAND_COUNT) {
                        shifted_reg = 1;
                } else if (token_instr->num_operands < TEST_INSTR_OPERAND_COUNT) {
                        fprintf(stderr, "Invalid number of operands: %d for computation instruction.", token_instr->num_operands);
                        exit(EXIT_FAILURE);
                }
		//assert(token_instr->num_operands == TEST_INSTR_OPERAND_COUNT);
		set_bits_to(&assembled_instr, SET_BIT, S_BIT);
	} else {
		//set destination register bits
		uint32_t dest_reg = strtoul(token_instr->operand[0] + 1, NULL, 0);
		set_bits_to(&assembled_instr, dest_reg, RD_START_BIT);

	}

	//instructions that compute results
	if (token_instr->opcode <= ORR) {
		if (token_instr->num_operands > COMPUTATION_INSTR_OPERAND_COUNT) {
			shifted_reg = 1;
		} else if (token_instr->num_operands < COMPUTATION_INSTR_OPERAND_COUNT) {
			fprintf(stderr, "Invalid number of operands: %d for computation instruction.", token_instr->num_operands);
			exit(EXIT_FAILURE);
		}	
		//set i to the correct position of operand2 in the operands array
		i = 2;
	}
	
	if (opcode != 0xd) {
		//set rn bits
		uint32_t rn = strtol(token_instr->operand[i - 1] + 1, NULL, 0);
		set_bits_to(&assembled_instr, rn, RN_START_BIT);
	}

	if (((token_instr->operand[i])[0] == '#') || ((token_instr->operand[i])[0] == '=')) {
		//set operand2 field to expression
		if (token_instr->opcode == LSL) { //[PC, #expression]
			char special_shift[4] = "lsl";
			encode_shifted_reg(token_instr->operand[0], special_shift, token_instr->operand[i], &assembled_instr);	
			return assembled_instr;
		} else {
			//not lsl, set operand2 to value of expression
			parse_expression(&assembled_instr, token_instr->operand[i]);
		}

		//set I bit (as the value is immediate)
		set_bits_to(&assembled_instr, SET_BIT, I_BIT);
	} else {
		//set rm bits
		uint32_t rm = strtol(token_instr->operand[i] + 1, NULL, 0);
		set_bits_to(&assembled_instr, rm, OP2_OFFSET_REG);
	}

	if (shifted_reg) {
		encode_shifted_reg(token_instr->operand[i], token_instr->operand[i+1], token_instr->operand[i+2], &assembled_instr);
	}

	return assembled_instr;
}

uint32_t assemble_multiply(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;

	//set rd register
	uint32_t rd = strtol(token_instr->operand[0] + 1, NULL, 0);
	set_bits_to(&assembled_instr, rd, RN_START_BIT);
	
	//set rm register
	uint32_t rm = strtol(token_instr->operand[1] + 1, NULL, 0);
	set_bits_to(&assembled_instr, rm, 0);

  
	//set rs register
	uint32_t rs = strtol(token_instr->operand[2] + 1, NULL, 0);
	set_bits_to(&assembled_instr, rs, 8);

	//set bits specific to multiply instruction
	set_bits_to(&assembled_instr, MULT_ONLY, MULT_ONLY_START_BIT);

	if (token_instr->num_operands == 4) {
		//set rn register field (if mla)
		uint32_t rn = strtol(token_instr->operand[3] + 1, NULL, 0);
		set_bits_to(&assembled_instr, rn, RD_START_BIT);

		//set A bit (multiply with accumulate)
		set_bits_to(&assembled_instr, SET_BIT, A_BIT);
	}

	//set condition field to ALWAYS
	set_bits_to(&assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);

	return assembled_instr;
}

uint32_t assemble_single_data_transfer(TokenizedInstruction *token_instr, uint32_t pc_address, uint32_t no_of_instructions, int32_t *eof_expressions, int *size_array) {
	uint32_t assembled_instr = 0;
	char *address_tokens[4];
	unsigned int num_tokens = 0;	
	
	//set bits specific to single data transfer
        set_bits_to(&assembled_instr, TRANSFER_INSTR, INSTR_FIELD);

        //set cond field
        set_bits_to(&assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);
	
	//set U flag (before trying optional cases with +/-)
	//set_bits_to(&assembled_instr, SET_BIT, U_BIT);
	
	//set rd bits
        uint32_t rd = strtol(token_instr->operand[0] + 1, NULL, 0);
        set_bits_to(&assembled_instr, rd, RD_START_BIT);

	if (token_instr->opcode == LDR) { //ldr instruction

		//set L flag
		set_bits_to(&assembled_instr, SET_BIT, L_BIT);

		if ((token_instr->operand[1])[0] == '=') { //address spec of form <=expression>
			int32_t expression = strtol(token_instr->operand[1] + 1, NULL, 0);
			set_bits_to(&assembled_instr, SET_BIT, U_BIT);
			if (expression <= MAX_REPRESENTABLE) { //use mov instruction
				token_instr->opcode = MOV;
				assembled_instr = assemble_data_processing(token_instr);
				return assembled_instr;
			} else {
				set_bits_to(&assembled_instr, SET_BIT, U_BIT);

				//add value of expression to end of file
				eof_expressions[*size_array] = expression;
				*size_array = *size_array + 1;

				//set P flag
				set_bits_to(&assembled_instr, SET_BIT, P_BIT);
				set_bits_to(&assembled_instr, SET_BIT, P_BIT);

				//calculate offset
				uint32_t offset = 0x4 * (no_of_instructions - pc_address + (*size_array-1)) - 0x8;
				
				//set rn (base register) to PC
				set_bits_to(&assembled_instr, PC_REGISTER, RN_START_BIT);

				//set offset
				set_bits_to(&assembled_instr, offset, OP2_OFFSET_REG);
				
				return assembled_instr;
			}
		}
	}

	//trim off square brackets from address specification
        char *address_spec = token_instr->operand[1] + 1;
        address_spec[strlen(address_spec) - 1] = '\0';

        //parse the address specification
        char *token = strtok(address_spec, ", \t");

        while (token != NULL) {
                address_tokens[num_tokens] = token;
                token = strtok(NULL, ", \t");
                num_tokens++;
        }

	//set rn bits
	uint32_t rn = strtol(address_tokens[0] + 1, NULL, 0);
	set_bits_to(&assembled_instr, rn, RN_START_BIT);
	
	//set offset
	uint32_t offset_exp = 0;

	if (token_instr->num_operands == 2) { //pre-indexing
		//set P flag
                set_bits_to(&assembled_instr, SET_BIT, P_BIT);
		if (num_tokens == 1) {
			set_bits_to(&assembled_instr, SET_BIT, U_BIT);
		} else if (num_tokens > 1) {	
			if (address_tokens[1][0] == 'r') {
				set_bits_to(&assembled_instr, SET_BIT, I_BIT);
				set_bits_to(&assembled_instr, SET_BIT, U_BIT);
				if (num_tokens == 4) {
					encode_shifted_reg(address_tokens[1], address_tokens[2], address_tokens[3], &assembled_instr);
				} else if(num_tokens == 2) {
					encode_shifted_reg(address_tokens[1],NULL, NULL, &assembled_instr);
				}
			} else if (address_tokens[1][0] == '#') {
				//offset_exp = strtol(address_tokens[1] + 1, NULL, 0);
				if (address_tokens[1][1] == '-') {
					offset_exp = strtol(address_tokens[1] + 2, NULL, 0);
				} else {
					offset_exp = strtol(address_tokens[1] + 1, NULL, 0);
					set_bits_to(&assembled_instr, SET_BIT, U_BIT);
				}
			}
		}
	} else if (token_instr->num_operands >= 3) { //post-indexing
		
		if (token_instr->operand[2][0] == 'r') {
			set_bits_to(&assembled_instr, SET_BIT, U_BIT);
                        set_bits_to(&assembled_instr, SET_BIT, I_BIT);
			if (token_instr->num_operands == 5) {
                        	encode_shifted_reg(token_instr->operand[2], token_instr->operand[3], token_instr->operand[4], &assembled_instr);
			} else if (token_instr->num_operands == 3) {
				encode_shifted_reg(token_instr->operand[2], NULL, NULL, &assembled_instr);
			}
                } else if (token_instr->operand[2][0] == '#') {
			if ((token_instr->operand[2][1] == '-')) {
				offset_exp = strtoul(token_instr->operand[2] + 2, NULL, 0);
			} else {
				set_bits_to(&assembled_instr, SET_BIT, U_BIT);
                		offset_exp = strtoul(token_instr->operand[2] + 1, NULL, 0);
			}
		}
	}
	
	if (offset_exp) {
		set_bits_to(&assembled_instr, offset_exp, OP2_OFFSET_REG);
	}

	return assembled_instr;
}

uint32_t assemble_branch(TokenizedInstruction *token_instr, uint32_t pc_address) {
	uint32_t assembled_instr = 0;
	uint32_t cond_field;
	uint32_t jump_address = strtoul(token_instr->operand[0], NULL, 16);

	switch(token_instr->opcode) {
		case BEQ:
			cond_field = 0x0;
			break;
		case BNE:
			cond_field = 0x1;
			break;
		case BGE:
			cond_field = 0xa;
			break;
		case BLT:
			cond_field = 0xb;
			break;
		case BGT:
			cond_field = 0xc;
			break;
		case BLE:
			cond_field = 0xd;
			break;
		case B:
			cond_field = ALWAYS_COND;
			break;
		default:
			fprintf(stderr, "Invalid opcode %x for branch instruction", token_instr->opcode);
            		exit(EXIT_FAILURE);
	}

	//set cond field
	set_bits_to(&assembled_instr, cond_field, COND_FIELD_START_BIT);
	
	int32_t offset = 0;
	//calculate offset (taking effects of ARM pipeline into account)

	uint32_t branch_address = pc_address * 4;

	if (jump_address > branch_address) {
		offset = (jump_address - branch_address - PIPELINE_EFFECT);
	} else {
		offset = (- (branch_address - jump_address + PIPELINE_EFFECT) / 4) & 0x00ffffff;
	}
	
	printf("jump address: %x\n", jump_address);
	printf("pc address: %x\n", pc_address);
	printf("offset: %x\n", offset);
	printf("bool: %d\n", jump_address > branch_address);
	
	
	//set offset field with signed 24 bit offset (after being shifted right 2 bits)
	set_bits_to(&assembled_instr, (uint32_t) offset, OP2_OFFSET_REG);

	//set condition field in assembled instruction
	set_bits_to(&assembled_instr, cond_field, COND_FIELD_START_BIT);
	
	//set bits specific to BRANCH instruction
	set_bits_to(&assembled_instr, BRANCH_INSTR, INSTR_FIELD);

	return assembled_instr;
}

uint32_t assemble_special(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;
	
	//set cond field to always
	set_bits_to(&assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);
	
	//set instruction field bits to input and print specifiers (these were chosen by programmers)
	set_bits_to(&assembled_instr, PRINT_INPUT_INSTR, INSTR_FIELD);

	//sets T bit based on format of operand (string or number)
	//T bit is set when the operand is in form of string
	switch(token_instr->opcode) {
		case PRINTS:
			set_bits_to(&assembled_instr, SET_BIT, T_BIT);
		case PRINTN:
			set_bits_to(&assembled_instr, SET_BIT, SPECIAL_I_BIT);
			break;
		case INPUTS:
			set_bits_to(&assembled_instr, SET_BIT, T_BIT);
		case INPUTN:
			set_bits_to(&assembled_instr, SET_BIT, SPECIAL_I_BIT);
			break;
		default:
			fprintf(stderr, "Invalid opcode %d for special instruction", token_instr->opcode);
                        exit(EXIT_FAILURE);
	}

	if (token_instr->operand[0][0] == 'r') {
		//R bit is set if the operand is the content of a register
		//if not set, operand is an immediate address
		set_bits_to(&assembled_instr, SET_BIT, R_BIT);
	}

	uint32_t mem_address = strtoul(token_instr->operand[0]+1, NULL, 0);
	set_bits_to(&assembled_instr, mem_address, OP2_OFFSET_REG);

	return assembled_instr;
}
		

void parse_expression(uint32_t *assembled_instr, char *operand_as_str) {
	uint32_t num_constant = strtoul((operand_as_str + 1), NULL, 0);
	
	if (num_constant > MAX_REPRESENTABLE) {
		//set operand immediate value with shift to immediate value
		encode_immediate_op2(assembled_instr, num_constant, SHIFT_TO_IMM);
	} else {
		//set operand equal to expression
		set_bits_to(assembled_instr, num_constant, OP2_OFFSET_REG);
	}

}

void encode_immediate_op2(uint32_t *assembled_instr, uint32_t immediate, unsigned int shift_start_bit) {
	uint32_t shift = 0;
	while (immediate % 2 == 0) {
		immediate >>=  1;
		shift++;
	}
	
	if (immediate > MAX_REPRESENTABLE) {
		perror("Immediate value could not be represented");
		exit(EXIT_FAILURE);
	}

	if (shift % 2 == 1) {
		immediate <<= 1;
	}
	
	uint32_t rot_val = MAX_ROTATION - (shift / 2);

	set_bits_to(assembled_instr, immediate, OP2_OFFSET_REG);
	set_bits_to(assembled_instr, rot_val, shift_start_bit);
}

void encode_shifted_reg(char *reg, const char *shift, char *shift_by, uint32_t *assembled_instr) {
	uint32_t shift_type = 0;
	uint32_t shift_val = 0;

	//assign register bits
        uint32_t reg_to_shift = strtoul(reg + 1, NULL, 0);
        set_bits_to(assembled_instr, reg_to_shift, OP2_OFFSET_REG);
	
	if (shift == NULL || shift_by == NULL) {
		return;
	}

	if(!strncmp(shift, "lsl\0", 3)) {
		shift_type = LSL_SHIFT;
	} else if (!strncmp(shift, "lsr\0", 3)) {
		shift_type = LSR_SHIFT;
	} else if (!strncmp(shift, "asr\0", 3)) {
		shift_type = ASR_SHIFT;
	} else if (!strncmp(shift, "ror\0", 3)) {
		shift_type = ROR_SHIFT;
	} else {
		fprintf(stderr, "Invalid shift type %s", shift);
	}

	//assign shift type bits
	set_bits_to(assembled_instr, shift_type, SHIFT_TYPE_BITS);

	if (shift_by[0] == '#') {
		uint32_t shift_int = strtol(shift_by + 1, NULL, 0);
		set_bits_to(assembled_instr, shift_int, SHIFT_BY_INT);
	} else {
		set_bits_to(assembled_instr, SET_BIT, SHIFT_INT_OR_REG_BIT);

		shift_val = strtoul(shift_by + 1, NULL, 0);
		set_bits_to(assembled_instr, shift_val, SHIFT_BY_REG);
	} 
}

