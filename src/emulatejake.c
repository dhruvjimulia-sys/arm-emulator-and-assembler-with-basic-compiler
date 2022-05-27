#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 65536
#define REGISTERS 17

struct Processor {
	uint8_t memory[MEM_SIZE];
	uint32_t registers[REGISTERS];
};	


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

uint8_t createMask(uint8_t start, uint8_t finish) {
	uint8_t r;
	r = ((1 << (finish - start)) - 1) << start
	return r;
}

void process_instructions(uint8_t* instruction) {
	uint8_t second4bits = createMask(24, 27) & *(instruction); 	
	if (second4bits == 1010) {
		//PC register = PC
		PC += createMask(0, 24) & *(instruction);
	}
}

void emulator_loop(uint8_t* instructions) {

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
