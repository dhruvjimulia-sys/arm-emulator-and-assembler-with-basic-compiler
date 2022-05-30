#include "type_definitions.h"

#ifndef EMUL_UTILS_H
#define EMUL_UTILS_H

instr_type get_instr_type(instr);

unsigned int get_opcode(uint32_t instr);

unsigned int immediate(uint32_t instr);

unsigned int get_rn_index(uint32_t instr);

unsigned int get_rd_index(uint32_t instr);

unsigned int operand2_or_offset(uint32_t instr);

uint32_t rotate_right(uint32_t n, unsigned int rot_val);

unsigned int get_rs_mult(uint32_t instr);

unsigned int get_rn_mult(uint32_t instr);

unsigned int get_rm_mult(uint32_t instr);

unsigned int get_branch_offset(uint32_t instr);

unsigned int set_cond_codes(uint32_t instr);

void set_n(uint32_t *cpsr, int32_t result);

void set_z(uint32_t *cpsr, int32_t result);

void set_c(uint32_t *cpsr, int32_t set_value);

void set_v(uint32_t *cpsr, int32_t result);

#endif
