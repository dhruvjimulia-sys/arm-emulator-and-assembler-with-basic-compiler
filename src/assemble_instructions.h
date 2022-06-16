#ifndef __ASSEMBLE_INSTRUCTIONS_H__
#define __ASSEMBLE_INSTRUCTIONS_H__

#include <stdint.h>
#include "assembler_type_definitions.h"

//assemble a data processing instruction
uint32_t assemble_data_processing(TokenizedInstruction *token_instr);

//assemble a multiply instruction
uint32_t assemble_multiply(TokenizedInstruction *token_instr);

//assemble a single data transfer instruction
uint32_t assemble_single_data_transfer(TokenizedInstruction *token_instr, uint32_t pc_address, uint32_t last_address, int32_t *eof_expressions);

//assemble a branch instruction
uint32_t assemble_branch(TokenizedInstruction *token_instr, uint32_t pc_address);

//set operand2 field or offset field to the value of the <#expression>
uint32_t parse_expression(char *operand_as_str);

//rotate value right given amount of times
uint32_t rotate_right(uint32_t n, unsigned int shift_amount);

//set necessary bits to 1 in a 32bit value
void set_bits_to(uint32_t instr, uint32_t input, unsigned int starting_at);

#endif
