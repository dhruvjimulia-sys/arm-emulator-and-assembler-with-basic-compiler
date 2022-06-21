#ifndef __ASSEMBLE_INSTRUCTIONS_H__
#define __ASSEMBLE_INSTRUCTIONS_H__

#include <stdint.h>
#include "assembler_type_definitions.h"

//set necessary bits to 1 in a 32bit value
void set_bits_to(uint32_t *instr, uint32_t input, unsigned int starting_at);

//assemble a data processing instruction
uint32_t assemble_data_processing(TokenizedInstruction *token_instr);

//assemble a multiply instruction
uint32_t assemble_multiply(TokenizedInstruction *token_instr);

//assemble a single data transfer instruction
uint32_t assemble_single_data_transfer(TokenizedInstruction *token_instr, uint32_t pc_address, uint32_t last_address, int32_t *eof_expressions, int *size_array);

//assemble a branch instruction
uint32_t assemble_branch(TokenizedInstruction *token_instr, uint32_t pc_address);

//assemble special instructions added for extension implementation
uint32_t assemble_special(TokenizedInstruction *token_instr);

//set operand2 field or offset field to the value of the <#expression>
void parse_expression(uint32_t *assembled_instr, char *operand_as_str);

//encodes an immediate value to fit inside the operand 2 field
void encode_immediate_op2(uint32_t *assembled_instr, uint32_t immediate, unsigned int shift_start_bit);

//encode a shifted register into assembled instruction
void encode_shifted_reg(char *reg, const char *shift, char *shift_by, uint32_t *assembled_instr);

#endif
