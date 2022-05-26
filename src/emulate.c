#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 65536
#define REGISTERS 17

struct Processor {
	uint8_t memory[MEM_SIZE];
	uint32_t registers[REGISTERS];
};	

uint8_t* load(char filename[]) {
	FILE *fp;
	uint64_t size;
	
	fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	uint64_t filesize = ftell(fp);
	size = filesize / 4;
	fseek(fp, 0, SEEK_SET);

	uint8_t* instructions = (uint8_t *) malloc(filesize);
	fread(instructions, 1, filesize, fp); 	
	fclose(fp);
	printf("%ld\n", size);
	
	for (int i = 0; i < filesize; i++) {
		for (int j = 7; j >= 0; j--) {
			uint8_t k;
			k = (instructions[i] & (1 << j)) >> j;
			printf("%d", k);
		}
		printf("\n");
	}
	

	for (int i = 0; i < filesize; i++) {
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
