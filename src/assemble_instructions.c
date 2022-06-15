#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "assemble_instructions.h"

#define RD_START_BIT 12
#define RN_START_BIT 16
#define PC_REGISTER 0xf
#define SET_BIT 0x1
#define S_BIT 20
#define L_BIT 20
#define I_BIT 25
#define A_BIT 21
#define P_BIT 24
#define OPCODE_START_BIT 21
#define COND_FIELD_START_BIT 28
#define ALWAYS_COND 0xe
#define TEST_INSTR_OPERAND_COUNT 2
#define COMPUTATION_INSTR_OPERAND_COUNT 3
#define SINGLE_OPERAND 1
#define DECIMAL 10
#define HEX 16
#define BRANCH_ONLY_START_BIT 24
#define BRANCH_ONLY 0xa
#define MULT_ONLY 0x9
#define MULT_ONLY_START_BIT 4
#define OP2_OR_OFFSET 0
#define SHIFT_TO_IMM 8
#define LSL_INTEGER_START_BIT 7

uint32_t assemble_data_processing(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;
	uint32_t opcode;

	//int i determines position of operand2 in the operands array (based on # of operands in the instructions)
	//preset to 1 (for tst, teq, cmp, mov)
	unsigned int i = 1;

	switch (token_instr->opcode) {
		case ANDEQ:
			set_bits_to(assembled_instr, 0, 0);
			return assembled_instr;
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
			printf("Unexpected opcode %x", token_instr->opcode);
			exit(EXIT_FAILURE);
	}

	set_bits_to(assembled_instr, opcode, OPCODE_START_BIT);

	//instructions that do not compute result, but set the CPSR flags
	if (opcode >= 0x8 && opcode <= 0xa) {
		assert(token_instr->num_operands == TEST_INSTR_OPERAND_COUNT);

		set_bits_to(assembled_instr, SET_BIT, S_BIT);
	} else {
		//set rd register bits in assembled instruction
                uint32_t rd = strtol(++(token_instr->operand[0]), NULL, DECIMAL);
                set_bits_to(assembled_instr, rd, RD_START_BIT);
	}

	//instructions that compute results
	if (token_instr->opcode <= ORR) {
		assert(token_instr->num_operands == COMPUTATION_INSTR_OPERAND_COUNT);
		
		//set i to the correct position of operand2 in the operands array
		i = 2;
	}
	
	if (!(opcode == 0xd)) {
		//set rn register bits in assembled instruction
		uint32_t rn = strtol(++(token_instr->operand[i - 1]), NULL, DECIMAL);
                set_bits_to(assembled_instr, rn, RN_START_BIT);
	}

	if ((token_instr->operand[i])[0] == '#') {
		//set operand2 field to expression
		if (token_instr->opcode == LSL) {
			//set integer part of [PC, #expression]
			uint32_t expression = strtoul(++(token_instr->operand[i]), NULL, HEX);
			set_bits_to(assembled_instr, expression, LSL_INTEGER_START_BIT);
		} else {
			//if not lsl, set expression
			set_expression(token_instr->operand[i], assembled_instr);
		}

		//set I bit (as the value is immediate)
		set_bits_to(assembled_instr, SET_BIT, I_BIT);
	}

	//set condition field to ALWAYS
	set_bits_to(assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);

	return assembled_instr;
}

uint32_t assemble_multiply(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;

	//set rd register
	uint32_t rd = strtol(++(token_instr->operand[0]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rd, RN_START_BIT);
	
	//set rm register
	uint32_t rm = strtol(++(token_instr->operand[1]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rm, 0);

	//set rs register
	uint32_t rs = strtol(++(token_instr->operand[2]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rs, 8);

	//set bits specific to multiply instruction
	set_bits_to(assembled_instr, MULT_ONLY, MULT_ONLY_START_BIT);

	if (token_instr->num_operands == 4) {
		//set rn register field (if mla)
		uint32_t rn = strtol(++(token_instr->operand[3]), NULL, DECIMAL);
        	set_bits_to(assembled_instr, rn, RD_START_BIT);

		//set A bit (multiply with accumulate)
		set_bits_to(assembled_instr, SET_BIT, A_BIT);
	}

	//set condition field to ALWAYS
	set_bits_to(assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);

	return assembled_instr;
}

uint32_t assemble_single_data_transfer(TokenizedInstruction *token_instr, uint32_t pc_address, uint32_t last_address) {
	uint32_t assembled_instr = 0;
	char **address_tokens = malloc(2 * sizeof(char *));
       	int num_tokens = 1;	
	
	//trim off square brackets from address specification
	char *address_spec = ++(token_instr->operand[1]);
	address_spec[strlen(address_spec) - 1] = '\0';

	//parse the address specification
	char *token = strtok(address_spec, ",");

	while (token != NULL) {
		address_tokens[num_tokens - 1] = token;
		token = strtok(NULL, ",");
		num_tokens++;
	}

	//ldr instruction
	if (token_instr->opcode == LDR) {

		//set L flag
                set_bits_to(assembled_instr, SET_BIT, A_BIT);

		//address spec of form <=expression>
		if ((token_instr->operand[1])[0] == '=') {
			uint32_t expression;
			expression = strtoul(++(token_instr->operand[1]), NULL, HEX);

			if (expression <= 0xff) {
				token_instr->opcode = MOV;
				return assemble_data_processing(token_instr);
			} else {
				//set pre-indexing flag bit
                		set_bits_to(assembled_instr, SET_BIT, P_BIT);

				//calculate offset
				uint32_t offset = last_address - (pc_address - 8);
						
				//set rn (base register) to PC
				set_bits_to(assembled_instr, PC_REGISTER, RN_START_BIT);

				//set offset bits
				set_bits_to(assembled_instr, offset, OP2_OR_OFFSET);
				
				return assembled_instr;
			}
		}
	}

	//set rn register field
	uint32_t rn = strtol(++(address_tokens[0]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rn, RN_START_BIT);

	if (num_tokens == 2) { //pre-indexing
		set_expression(address_tokens[1], assembled_instr);
		
		//set pre-indexing flag bit
		set_bits_to(assembled_instr, SET_BIT, P_BIT);
	
	} else { //post-indexing
		set_expression(token_instr->operand[2], assembled_instr);

	}
	
	//set rd register field
	uint32_t rd = strtol(++(token_instr->operand[0]), NULL, DECIMAL);
        set_bits_to(assembled_instr, rd, RD_START_BIT);	

	return assembled_instr;
}

uint32_t assemble_branch(TokenizedInstruction *token_instr, uint32_t pc_address) {
	uint32_t assembled_instr = 0;
	uint32_t cond_field;
	uint32_t jump_address = atoi(token_instr->operand[0]);
	uint32_t offset;

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
	}
	//set condition field in assembled instruction
        set_bits_to(assembled_instr, cond_field, COND_FIELD_START_BIT);
	
	//calculate offset (-8 for the effects of ARM pipeline)
	offset = (uint32_t) ((jump_address - (pc_address - 8)) >> 2);
	
	//set offset field with signed 24 bit offset (after being shifted right 2 bits)
	set_bits_to(assembled_instr, offset, OP2_OR_OFFSET);

	//set condition field in assembled instruction
        set_bits_to(assembled_instr, cond_field, COND_FIELD_START_BIT);
	
	//set bits specific to BRANCH instruction
        set_bits_to(assembled_instr, BRANCH_ONLY, BRANCH_ONLY_START_BIT);

	return assembled_instr;
}

//set operand2 in assembled instruction if operand2 is an immediate expression
void set_expression(char *operand_as_str, uint32_t assembled_instr) {
	uint32_t num_constant = strtoul(++(operand_as_str), NULL, HEX);
	uint32_t shift_imm = 0;

	//replace magic number?
	if (num_constant > 0xff) {
		
		//calculate shift to immediate amount (rotate value)
		while (num_constant > 0xff) {
			shift_imm++;
			rotate_right(num_constant, 2);
		}
		
		//set operand immediate value with shift to immediate value
		set_bits_to(assembled_instr, num_constant, OP2_OR_OFFSET);
		set_bits_to(assembled_instr, shift_imm, SHIFT_TO_IMM); 
	} else {
		//set operand equal to expression
		set_bits_to(assembled_instr, num_constant, OP2_OR_OFFSET);
	}
}

uint32_t rotate_right(uint32_t n, unsigned int shift_amount) {
  if ((shift_amount &= sizeof(n) * 8 - 1) == 0)
    return n;
  return (n << shift_amount) | (n >> (sizeof(n) * 8 - shift_amount));
}

void set_bits_to(uint32_t instr, uint32_t input, unsigned int starting_at) {
        instr |= (input << starting_at);
}
