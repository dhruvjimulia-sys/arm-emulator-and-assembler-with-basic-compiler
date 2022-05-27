#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 65536
#define REGISTERS 17
#define PC_REGISTER 15
#define BYTES_PER_INSTRUCTION 4

struct Processor {
	uint8_t memory[MEM_SIZE];
	uint32_t registers[REGISTERS];
} processor;	


uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

uint8_t* load(char filename[]) {
	FILE *fp;
	
	fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t* instructions = (uint8_t *) malloc(filesize);
	fread(instructions, 1, filesize, fp); 	
	fclose(fp);
	printf("%ld\n", filesize/4);
	
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

bool process_instructions(uint8_t* instruction) {
	return true;
}

void clear_array(uint8_t* arr, uint64_t length) {
	for (int i = 0; i < length; i++) {
		arr[i] = 0;
	}
}

bool is_all_zero(uint8_t* arr, uint64_t length) {
	for (int i = 0; i < length; i++) {
		if (arr[i] != 0) {
			return false;
		}
	}
	return true;
}

void emulator_loop(uint8_t* instructions) {
	uint8_t* fetched = (uint8_t*) malloc(BYTES_PER_INSTRUCTION);
	uint8_t* decoded = (uint8_t*) malloc(BYTES_PER_INSTRUCTION);
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
	return EXIT_SUCCESS;
}
