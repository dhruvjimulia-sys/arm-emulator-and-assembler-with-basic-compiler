#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

#ifndef INSTRUCTONS_H
#define INSTRUCTIONS_H

int32_t shift(uint32_t n, uint32_t shift_type, uint32_t shift_amount,
			bool set_cpsr, uint32_t *cpsr_reg);

uint32_t shift_rm_register(uint32_t *instr, struct Processor processor, bool set_cpsr);

void execute_data_processing_instruction(struct Processor processor, uint32_t *instr);

void execute_branch_instruction(uint32_t *instr);

void execute_multiply_instruction(struct Processor processor, uint32_t *instr);

void execute_single_data_transfer(struct Processor processor, uint32_t *instr);

#endif
