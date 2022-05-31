#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
// "utils.h" requires "type_definitions.h"

#ifndef UTILS_H
#define UTILS_H

#define PC_REGISTER 15
#define CPSR_REGISTER 16

int32_t sign_extend_26(int32_t extendable);

uint8_t reverse(uint8_t b);

bool extract_bit(uint8_t position, uint32_t* instruction);

uint32_t create_mask(uint8_t start, uint8_t finish, uint32_t* instruction);

void clear_array(uint8_t* arr, uint64_t length);

bool is_all_zero(uint8_t* arr, uint64_t length);

instr_type get_instr_type(uint32_t *instr);

uint32_t rotate_right(uint32_t n, unsigned int rot_val);

void set_n(uint32_t *cpsr, int32_t result);

void set_z(uint32_t *cpsr, int32_t result);

void set_c(uint32_t *cpsr, int32_t set_value);

void set_v(uint32_t *cpsr, int32_t result);

#endif
