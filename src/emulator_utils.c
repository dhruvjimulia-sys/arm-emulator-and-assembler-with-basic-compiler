#include "emulator_utils.h"
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#define Z_FLAG 0x40000000
#define V_FLAG 0x10000000
#define C_FLAG 0x20000000
#define MULT_BITS 0x00000009


bool condition_check(uint32_t type, struct Processor* processor) {

        static const uint8_t N_POS = 0;
        static const uint8_t Z_POS = 1;
        static const uint8_t V_POS = 3;

        bool n = extract_bit(N_POS, processor->registers + CPSR_REGISTER);
        bool z = extract_bit(Z_POS, processor->registers + CPSR_REGISTER);
        bool v = extract_bit(V_POS, processor->registers + CPSR_REGISTER);

        switch (type) {
                case eq :
                        return z;
                case ne :
                        return !z;
                case ge :
                        return n == v;
                case lt :
                        return n != v;
                case gt :
                        return !z && (n == v);
                case le :
                        return z || (n != v);
                case al :
                        return true;
                default :
                        printf("wrong condition code");
                        return false;
        }
}


int32_t sign_extend(int32_t extendable, uint8_t num_bits) {
	uint32_t m = 1 << (num_bits - 1);
	return (extendable ^ m) - m;
}

uint32_t change_endianness(uint32_t index, uint8_t offset) {
	return (((index - offset) / BYTES_PER_INSTRUCTION) + 1) * BYTES_PER_INSTRUCTION - 1 - ((index - offset) % BYTES_PER_INSTRUCTION) + offset;
}

uint32_t reverse_bits(uint32_t num, uint8_t num_bits) {
	uint32_t reverse_num = 0;
	for (uint8_t i = 0; i < num_bits; i++) {
		if ((num & (1 << i))) {
			reverse_num |= 1 << ((num_bits - 1) - i);
		}
	}
	return reverse_num;
}

bool extract_bit(uint8_t position, uint32_t* instruction) {
	return create_mask(position, position, instruction) == 1;
}

uint32_t create_mask(uint8_t start, uint8_t finish, uint32_t* instruction) {
	assert(start <= finish);
	start = BITS_PER_INSTRUCTION - 1 - start;
	finish = BITS_PER_INSTRUCTION - 1 - finish;
	uint32_t r = ((1 << (start - finish + 1)) - 1) << finish;
	return reverse_bits((r & *instruction) >> finish, start - finish + 1);
}

void clear_array(uint8_t* arr, uint64_t length) {
	for (int i = 0; i < length; i++) {
		arr[i] = 0;
	}
}

bool is_all_zero(uint8_t* arr, uint32_t length) {
	for (int i = 0; i < length; i++) {
		if (arr[i] != 0) {
			return false;
		}
	}
	return true;
}

instr_type get_instr_type(uint32_t *instr) {
	uint32_t type_bits = create_mask(26, 27, instr);

	switch(type_bits) {
		case 0:
			if (create_mask(4, 7, instr) == MULT_BITS) {
				return MULTIPLY;
			} else {
				return DATA_PROCESS;
			}
		case 1:
			return TRANSFER;
		case 2:
			return BRANCH;
		default:
			exit(EXIT_FAILURE);
	}

}

uint32_t rotate_right(uint32_t n, unsigned int rot_val) {
	const unsigned int mask = (8*sizeof(n) - 1);
	rot_val &= mask;
	return (n >> rot_val) | (n << ((-rot_val)&mask));
}

void set_n(uint32_t *cpsr, int32_t result) {
	*cpsr |= (result & (1<<31));
}

void set_z(uint32_t *cpsr, int32_t result) {
	if (result == 0) {
		*cpsr |= Z_FLAG;
	} else {
		*cpsr &= ~Z_FLAG;
	}
}

void set_c(uint32_t *cpsr, int32_t set) {
	if (set) {
		*cpsr |= C_FLAG;
	}
}

void set_v(uint32_t *cpsr, int32_t result) {
	*cpsr |= V_FLAG;
}

void print_instruction_bits(uint32_t* instruction) {
	for (int j = BITS_PER_INSTRUCTION - 1; j >= 0; j--) {
		printf("%d", extract_bit(j, instruction));
	}
	printf("\n");
}
