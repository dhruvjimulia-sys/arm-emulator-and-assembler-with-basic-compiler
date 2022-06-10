#include <stdint.h>
#include "tokenizer.h"
#include "assembler_utils.h"

#ifndef ASSEMBLE_INSTRUCTIONS_H
#define ASSEMBLE_INSTRUCTIONS_H

//assemble a data processing instruction
uint32_t assemble_data_processing(TokenizedInstruction *token_instr);

//assemble a multiply instruction
uint32_t assemble_multiply(TokenizedInstruction *token_instr);

//assemble a single data transfer instruction
uint32_t assemble_single_data_transfer(TokenizedInstruction *token_instr, uint32_t data_address, uint32_t total_of_instr);

//assemble a branch instruction
uint32_t assemble_branch(TokenizedInstruction *token_instr, hash_table *symbol_table, uint32_t data_address);

//set operand2 field or offset field to the value of the <#expression>
uint32_t set_expression(char *operand_as_str, uint32_t assembled_instr);

#endif
