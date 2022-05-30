#include "type_definitions.h"

#ifndef EMUL_UTILS_H
#define EMUL_UTILS_H

instr_type get_instr_type(instr);

uint32_t rotate_right(uint32_t n, unsigned int rot_val);

void set_n(uint32_t *cpsr, int32_t result);

void set_z(uint32_t *cpsr, int32_t result);

void set_c(uint32_t *cpsr, int32_t set_value);

void set_v(uint32_t *cpsr, int32_t result);

#endif
