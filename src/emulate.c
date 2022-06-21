#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "emulate.h"

#define BYTES_PER_INSTRUCTION 4
#define BITS_PER_INSTRUCTION 32

void load(char filename[], struct Processor* processor) {
	FILE *fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	uint8_t instructions[filesize];
	fread(instructions, 1, filesize, fp);
	fclose(fp);
	for (int i = 0; i < filesize; i++) {
		processor->memory[i] = reverse_bits(instructions[change_endianness(i, 0)], CHAR_BIT);
	}
}

//return true: clear pipeline
//return false: leave pipeline intact
bool process_instructions(uint8_t* instruction_bytes, struct Processor* processor) {
	uint32_t *instruction = (uint32_t *) instruction_bytes;
	switch (get_instr_type(instruction)) {
		case BRANCH :
			return execute_branch_instruction(processor, instruction);
		case TRANSFER :
			execute_single_data_transfer(processor, instruction);
			return false;
		case MULTIPLY :
			execute_multiply_instruction(processor, instruction);
			return false;
		case DATA_PROCESS :
			execute_data_processing_instruction(processor, instruction);
			return false;
		case PRINT :
			print_basic(processor, instruction);
			return false;
		case INPUT :
			input_basic(processor, instruction);
			return false;
		default:
			perror("Instruction not identified");
			return false;
	}
}


void emulator_loop(struct Processor* processor) {
	uint8_t* instructions = processor->memory;
	uint8_t fetched[BYTES_PER_INSTRUCTION];
	uint8_t decoded[BYTES_PER_INSTRUCTION];
	bool decoded_valid = false;
	bool execute_valid = false;

	while (true) {
		/* Execute */
		if (execute_valid) {
			if (process_instructions(decoded, processor)) {
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
				processor->registers[PC_REGISTER] += BYTES_PER_INSTRUCTION;
				break;
			}
			execute_valid = true;
		}

		/* Fetch */
		for (int i = 0; i < BYTES_PER_INSTRUCTION; i++) {
			fetched[i] = instructions[processor->registers[PC_REGISTER] + i];
		}
		decoded_valid = true;

		processor->registers[PC_REGISTER] += BYTES_PER_INSTRUCTION;
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
			printf(": %10d (0x%08x)\n", processor->registers[j], processor->registers[j]);
		}
	}
	printf("Non-zero memory:\n");
	for (uint32_t i = 0; i < MEM_SIZE; i += 4) {
		if (!is_all_zero(processor->memory + i, BYTES_PER_INSTRUCTION)) {
			printf("0x%08x: 0x", i);
			for (uint32_t j = 0; j < 4; j++) {
				printf("%02x", reverse_bits(processor->memory[change_endianness(i + j, 0)], CHAR_BIT));
			}
			printf("\n");
		}
	}
}

int main(int argc, char **argv) {
	struct Processor processor = {{0}, {0}};
	if (argc != 2) {
		if (argc > 2) {
			printf("Too many arguments supplied\n");
		} else {
			printf("No arguments\n");
		}
		return EXIT_FAILURE;
	}
	load(argv[1], &processor);
	emulator_loop(&processor);
	return EXIT_SUCCESS;
}
