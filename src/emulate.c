#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 65536
#define REGISTERS 17

struct Processor {
	uint8_t memory[MEM_SIZE];
	uint32_t registers[REGISTERS];
};	

uint32_t* load(char filename[]) {
	FILE *fp;
	uint64_t size;
	
	fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t filesize = ftell(fp);
	size = filesize / 4 + (filesize % 4 != 0);
	fseek(fp, 0, SEEK_SET);

	uint32_t* instructions = (uint32_t *) malloc(size);
	fread(instructions, 4, size, fp); 	
	fclose(fp);
	printf("%ld\n", size);
	for (int i = 0; i < size; i++) {
		printf("%u\n", instructions[i]);
	}

	return instructions;
}

int main(int argc, char **argv) {
	if (argc == 2) {
		load(argv[1]);
	} else if (argc > 2) {
		printf("Too many arguments supplied\n");
	} else {
		printf("No arguments\n");
	}
	return EXIT_SUCCESS;
}
