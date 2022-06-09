#include <stdint.h>
#include "tokenizer.h"
#include "assembler_utils.h"

#define RD_START_BIT 16
#define RN_START_BIT 12
#define SET_BIT 0x1
#define S_BIT 20
#define L_BIT 20
#define I_BIT 25
#define A_BIT 21
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
#define OP2_OR_OFFSET 0
#define SHIFT_TO_IMM 8

uint32_t assemble_data_processing(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;
	uint32_t opcode;
	uint32_t operand2;

	//int i will be set to position of operand 2 based on number of operands in different instructions
	unsigned int i;

	switch (token_instr->opcode) {
		case ANDEQ:
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

		//lookup(opcode, symbol_table);
		//how would we go about ldr and str
	}

	set_bits_to(assembled_instr, opcode, OPCODE_START_BIT);

	if (opcode >= 0x8 && opcode <= 0xa) {
		assert(token_instr->num_operands == TEST_INSTR_OPERAND_COUNT);
		//sets i to the correct position of operand 2 in the operands array
		i = 1;

		set_bits_to(assembled_instr, SET_BIT, S_BIT);
	}

	if (token_instr->opcode <= ORR) {
		assert(token_instr->num_operands == COMPUTATION_INSTR_OPERAND_COUNT);
		//sets i to the correct position of operand 2 in the operands array
		i = 2;

		//set rd register bits in assembled instruction
		uint32_t rd = strtol(++(token_instr.operand[0]), NULL, DECIMAL);
		set_bits_to(assembled_instr, rd, RN_START_BIT);
	}

	if (!(opcode == 0xd)) {
		//set rn register bits in assembled instruction
		uint32_t rn = strtol(++(token_instr.operand[1]), NULL, DECIMAL);
                set_bits_to(assembled_instr, rd, RD_START_BIT);
	}

	if ((token_instr->operand[i])[0] == '#') {
		//set operand2 field to expression
		set_expression(token_instr->operand[i], assembled_instr);

		//set I bit (as the value is immediate)
		set_bits_to(assembled_instr, SET_BIT, I_BIT);
	} else {
		//implement register and shift type (optional)
	}
	
	//set condition field to always
	set_bits_to(assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);
}

uint32_t assemble_multiply(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0;

	uint32_t rd = strtol(++(token_instr.operand[0]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rd, RN_START_BIT);

	uint32_t rm = strtol(++(token_instr.operand[1]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rm, 0);

	uint32_t rs = strtol(++(token_instr.operand[2]), NULL, DECIMAL);
	set_bits_to(assembled_instr, rs, 8);

	if (token_instr->num_operands == 4) {
		//set rn register field (if mla)
		uint32_t rn = strtol(++(token_instr.operand[3]), NULL, DECIMAL);
        	set_bits_to(assembled_instr, rn, RD_START_BIT);

		//set A bit
		set_bits_to(assembled_instr, SET_BIT, A_BIT);
	}

	//set condition field
	set_bits_to(assembled_instr, ALWAYS_COND, COND_FIELD_START_BIT);
}

uint32_t assemble_single_data_transfer(TokenizedInstruction *token_instr) {
	uint32_t assembled_instr = 0; 
	
	if (token_instr->opcode == LDR) {
		if ((token_instr->operand[1])[0] == '=') {
			uint32_t expression;
			expression = strtoul(++(token_instr->operand[1]), NULL, HEX);

			if (expression <= 0xff) {
				token_instr->opcode = MOV;
				return assemble_data_processing(token_instr);
			} else {
				//how to calcute offset from PC
				//FINISH!!
				uint32_t offset;
		}
		
		//set L bit (as this is an ldr instruction)
		set_bits_to(assembled_instr, SET_BIT, A_BIT);
	}

	//set rd register field
	uint32_t rd = strtol(++(token_instr.operand[0]), NULL, DECIMAL);
        set_bits_to(assembled_instr, rd, RD_START_BIT);	
}

uint32_t assemble_branch(TokenizedInstruction *token_instr, hash_table *symbol_table) {
	uint32_t assembled_instr = 0;
	uint32_t cond_field;
	uint32_t jump_address;
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
		case BAL:
		case B:
			cond_field = ALWAYS_COND;
	
	//create isLabel function!!!!!!
	if (isLabel(token_instr->operand[1])) {
		jump_address = lookup(token_instr->operand[1], symbol_table);
	} else {
		jump_address = strtol(token_instr->operand[1], NULL, HEX);
	}

	//compute offset -> need access to current address (PC)
	//The assembler should compute the offset between the current address and the label, taking into account the off-by-8 bytes effect of ARM pipeline
	//This signed offset should be 26 bits in length, before being shifted right two bits and having the lower 24 bits stored in the Offset field.
	//FINISH
	
	//set offset field with signed 24 bit offset (after being shifted right 2 bits)
	set_bits_to(assembled_instr, offset, OP2_OR_OFFSET);

	//set condition field in assembled instruction
        set_bits_to(assembled_instr, cond_field, COND_FIELD_START_BIT);

        //check if expression is address or label??
        set_bits_to(assembled_instr, BRANCH_ONLY, BRANCH_ONLY_START_BIT);
}

void set_bits_to(uint32_t *instr, uint32_t input, unsigned int starting_at) {
	*instr |= (input << starting_at);
}

//set operand2 in assembled instruction if operand2 is an immediate expression
uint32_t set_expression(char *operand_as_str, uint32_t assembled_instr) {
	uint32_t num_constant = strtoul(++(operand_as_str), NULL, HEX);
	uint32_t shift_imm = 0;

	//replace magic number?
	if (num_constant > 0xff) {
		uint32_t rotate_amount = 0;

		//calculate shift to immediate amount (rotate value)
		while (num_constant > 0xff) {
			shift_imm++;
			rotate_right(num_consant, 2);
		}
		
		set_bits_to(assembled_instr, num_constant, OP2_OR_OFFSET);
		set_bits_to(assembled_instr, shift_imm, SHIFT_TO_IMM); 
	} else {
		set_bits_to(assembled_instr, num_constant, OP2_OR_OFFSET);
	}
}
