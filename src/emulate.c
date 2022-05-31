#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 65536
#define REGISTERS 17
#define PC_REGISTER 15
#define CPSR_REGISTER 16
#define BYTES_PER_INSTRUCTION 4
#define BITS_PER_INSTRUCTION 32

struct Processor {
	uint8_t memory[MEM_SIZE];
	uint32_t registers[REGISTERS];
} processor;

enum Flags {
	eq = 0,
	ne = 1,
	ge = 10,
	lt = 11,
	gt = 12,
	le = 13,
	al = 14,
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

uint32_t create_mask(uint8_t start, uint8_t finish, uint32_t* instruction) {
        uint32_t r;
        r = ((1 << (finish - start)) - 1) << start;
        return (r & *(instruction)) >> start;
}


bool extract_bit(uint8_t position, uint32_t* instruction) {
	return create_mask(position, position, instruction) == 1;
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

int32_t sign_extend_26(int32_t extendable) {
	static const int SIGN_EXTEND = 4227858432U;
	return extendable + SIGN_EXTEND;
}

//return true: clear pipeline
//return false: leave pipeline intact
bool process_instructions(uint8_t* instruction_bytes) {
	uint32_t *instruction = (uint32_t *) realloc(instruction_bytes, BITS_PER_INSTRUCTION);
        uint32_t first4bits = create_mask(31, 28, instruction);
        uint32_t second4bits = create_mask(24, 27, instruction); 
        static const uint8_t PIPELINE_CORRECTION = 8;
	// Branch
	// Remember to change 10 to the enum Nada made
        if (second4bits == 10) {
                if (condition_check(first4bits)) {
                        int32_t offset = (int32_t) (create_mask(0, 23, instruction)) << 2;
                        if (offset < 0) {
                                offset = sign_extend_26(offset);
                        }
		
                        processor.registers[PC_REGISTER] += offset - PIPELINE_CORRECTION;
			return true;
                }
        }
	return false;
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
	return EXIT_SUCCESS;
}
