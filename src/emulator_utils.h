#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "type_definitions.h"

#ifndef EMULATOR_UTILS_H
#define EMULATOR_UTILS_H

#define SP_REGISTER 13
#define LR_REGISTER 14
#define PC_REGISTER 15
#define CPSR_REGISTER 16
#define BYTES_PER_INSTRUCTION 4
#define BITS_PER_INSTRUCTION 32

//check condition flags register
bool condition_check(uint32_t first4bits, struct Processor* processor);

//sign extension of to 32 bit value
int32_t sign_extend(int32_t extendable, uint8_t num_bits);

//change from little endian to big endian
uint32_t change_endianness(uint32_t index, uint8_t offset);

//reverse loaded bytes
uint32_t reverse_bits(uint32_t b, uint8_t num_bits);

//extract bit at specified position and return corresponding boolean value
bool extract_bit(uint8_t position, uint32_t* instruction);

//create a bit mask for specified bit range of a 32 bit instruction
//returns 32 bit binary value of specified bits
uint32_t create_mask(uint8_t start, uint8_t finish, uint32_t* instruction);

//clear (zero out) the contents of the array
void clear_array(uint8_t* arr, uint64_t length);

//check if array contains all zeroes
bool is_all_zero(uint8_t* arr, uint32_t length);

//return instruction type
instr_type get_instr_type(uint32_t *instr);

//rotate given integer right by the specified rotation value
uint32_t rotate_right(uint32_t n, unsigned int rot_val);

//set N flag
void set_n(uint32_t *cpsr, int32_t result);

//set Z flag
void set_z(uint32_t *cpsr, int32_t result);

//set C flag
void set_c(uint32_t *cpsr, int32_t set_value);

//set V flag
void set_v(uint32_t *cpsr, int32_t result);

//print instruction bits
void print_instruction_bits(uint32_t* instruction);

#endif
