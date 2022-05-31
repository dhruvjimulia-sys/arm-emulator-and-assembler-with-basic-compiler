#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "emulate.h"
#include "type_definitions.h"
#include "utils.h"
#include "instructions.h"

#define BYTES_PER_INSTRUCTION 4
#define BITS_PER_INSTRUCTION 32

uint8_t* load(char filename[]) {
	FILE *fp;

	fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t* instructions = malloc(filesize);
	assert(instructions);
	fread(instructions, 1, filesize, fp);
	fclose(fp);

	printf("%ld\n", filesize / BYTES_PER_INSTRUCTION);

	for (int i = 0; i < filesize; i++) {
		instructions[i] = reverse(instructions[i]);
		for (int j = 0; j < 8; j++) {
			uint8_t k;
			k = (instructions[i] & (1 << j)) >> j;
			printf("%d", k);
		}
		printf("\n");
	}
	return instructions;
}

bool condition_check(uint32_t type) {

	static const uint8_t N_POS = 31;
	static const uint8_t Z_POS = 30;
	static const uint8_t C_POS = 29;
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

//return true: clear pipeline
//return false: leave pipeline intact
bool process_instructions(uint8_t* instruction_bytes, instr_type type) {
	uint32_t *instruction = realloc(instruction_bytes, BITS_PER_INSTRUCTION);
        uint32_t first4bits = create_mask(31, 28, instruction);
        static const uint8_t PIPELINE_CORRECTION = 8;
	// Branch
	// Remember to change 10 to the enum Nada made
        switch (type) {
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
			//
		case MULTIPLY :
			//
		case DATA_PROCESS :
			//
		default:
			exit(EXIT_FAILURE);
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
			if (is_all_zero(decoded, BYTES_PER_INSTRUCTION)) {
				break;
			}
			for (int i = 0; i < BYTES_PER_INSTRUCTION; i++) {
				decoded[i] = fetched[i];
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
	printf("Memory:\n");
	for (int i = 0; i < MEM_SIZE; i++) {
		if (processor.memory[i] != 0) {
			printf("Memory at position %X: ", i);
			printf("%X\n", processor.memory[i]);
		}
	}
	printf("\nRegisters:\n");
	for (int j = 0; j < REGISTERS; j++) {
		printf("Register %d has value: ", j);
		printf("%X\n", processor.registers[j]);
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
	uint8_t* instructions = load(argv[1]);
	emulator_loop(instructions);
	free(instructions);
	return EXIT_SUCCESS;
}
