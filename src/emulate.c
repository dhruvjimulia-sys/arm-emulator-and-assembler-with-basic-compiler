#include "type_definitions.h"
#include "utils.h"
#include "instructions.h"
#include "emulate.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define BYTES_PER_INSTRUCTION 4
#define BITS_PER_INSTRUCTION 32

struct Processor processor;

void load(char filename[]) {
	FILE *fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	uint8_t instructions[filesize];
	fread(instructions, 1, filesize, fp);
	fclose(fp);

	for (int i = 0; i < filesize; i++) {
		processor.memory[i] = reverse_bits(instructions[change_endianness(i)], CHAR_BIT);
	}
}

bool condition_check(uint32_t type) {

	static const uint8_t N_POS = 31;
	static const uint8_t Z_POS = 30;
	// static const uint8_t C_POS = 29;
	static const uint8_t V_POS = 28;

	bool n = extract_bit(N_POS, &processor.registers[CPSR_REGISTER]);
 	bool z = extract_bit(Z_POS, &processor.registers[CPSR_REGISTER]);
	//bool c = extract_bit(C_POS, &processor.registers[CPSR_REGISTER]);
 	bool v = extract_bit(V_POS, &processor.registers[CPSR_REGISTER]);

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

void print_instruction_bits(uint32_t* instruction) {
	for (int j = BITS_PER_INSTRUCTION - 1; j >= 0; j--) {
		printf("%d", extract_bit(j, instruction));
	}
	printf("\n");
}

//return true: clear pipeline
//return false: leave pipeline intact
bool process_instructions(uint8_t* instruction_bytes) {
	uint32_t *instruction = (uint32_t *) instruction_bytes;
    uint32_t first4bits = create_mask(31, 28, instruction);
    static const uint8_t PIPELINE_CORRECTION = 8;
	switch (get_instr_type(instruction)) {
		case BRANCH :
			if (condition_check(first4bits)) {
				int32_t offset = (int32_t) (create_mask(0, 23, instruction)) << 2;
				if (offset < 0) {
					offset = sign_extend_26(offset);
				}
				processor.registers[PC_REGISTER] += offset - PIPELINE_CORRECTION;
				return true;
			}
			return false;
		case TRANSFER :
			execute_single_data_transfer(&processor, instruction);
			return false;
		case MULTIPLY :
			execute_multiply_instruction(&processor, instruction);
			return false;
		case DATA_PROCESS :
			execute_data_processing_instruction(&processor, instruction);
			return false;
		default:
			perror("Instruction not identified");
			return false;
	}		
}

void emulator_loop(uint8_t* instructions) {
	uint8_t fetched[BYTES_PER_INSTRUCTION];
	uint8_t decoded[BYTES_PER_INSTRUCTION];
	bool decoded_valid = false;
	bool execute_valid = false;

	while (true) {
		/* Execute */
		if (execute_valid) {
			if (process_instructions(decoded)) {
				clear_array(decoded, BYTES_PER_INSTRUCTION);
				clear_array(fetched, BYTES_PER_INSTRUCTION);
				decoded_valid = false;
				execute_valid = false;
			}
		}

		/* Decode */
		if (decoded_valid) {
			for (int i = 0; i < BYTES_PER_INSTRUCTION; i++) {
				decoded[i] = fetched[i];
			}
			if (is_all_zero(decoded, BYTES_PER_INSTRUCTION)) {
				processor.registers[PC_REGISTER] += BYTES_PER_INSTRUCTION;
				break;
			}
			execute_valid = true;
		}

		/* Fetch */
		for (int i = 0; i < BYTES_PER_INSTRUCTION; i++) {
			fetched[i] = instructions[processor.registers[PC_REGISTER] + i];
		}
		decoded_valid = true;

		processor.registers[PC_REGISTER] += BYTES_PER_INSTRUCTION;
	}

	/* Print processor status */
	printf("Registers:\n");
	for (int j = 0; j < REGISTERS; j++) {
		if (j != SP_REGISTER && j != LR_REGISTER) {
			if (j == PC_REGISTER) {
				printf("PC  ");
			} else if (j == CPSR_REGISTER) {
				printf("CPSR");
			} else {
				printf("$%-3d", j);
			}
			printf(":%10d (0x%08x)\n", processor.registers[j], processor.registers[j]);
		}
	}
	printf("Non-zero memory:\n");	
	for (uint32_t i = 0; i < MEM_SIZE; i += 4) {
		if (!is_all_zero(processor.memory + i, BYTES_PER_INSTRUCTION)) {
			printf("0x%08x: 0x", i);
			for (uint32_t j = 0; j < 4; j++) {
				printf("%02x", reverse_bits(processor.memory[change_endianness(i + j)], CHAR_BIT));
			}
			printf("\n");
		}
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		if (argc > 2) {
			printf("Too many arguments supplied\n");
		} else {
			printf("No arguments\n");
		}
		return EXIT_FAILURE;
	}
	load(argv[1]);
	emulator_loop(processor.memory);
	return EXIT_SUCCESS;
}
