#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "assembler_type_definitions.h"
#include "symbol_table.h"
#include "tokenizer.h"
#include "assemble_instructions.h"
#include <errno.h>

#define MAX_LINE_SIZE 512

uint32_t reverse(uint32_t value){
	return (((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) <<  8) |
            ((value & 0x00FF0000) >>  8) |
            ((value & 0xFF000000) >> 24));
}

//printing binary instructions onto the destination file
void binary_writer(char* dest_file, uint32_t result){
	FILE *fp = fopen(dest_file,"a");
	assert(fp != NULL);
	fwrite(&result,4,1,fp);
	if (ferror(fp)){
		printf("Error in writing to file\n");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
}


void binary_writer_array(char *dest_file, int32_t *result_array, int size){
	FILE *fp = fopen(dest_file,"a");
	assert(fp != NULL);
	
	for (int i=0; i < size; i++){
		uint32_t result = result_array[i];
		fwrite(&result,4,1,fp);
	}

	if (ferror(fp)){
		printf("Error in writing array elements to file\n");
	}
	fclose(fp);
}

bool islabel(char *line){
	return line[strlen(line)-1] == ':';
}

void call_instruction(TokenizedInstruction *instruction, hash_table *symbol_table, uint32_t pc, uint32_t last_address, char *dest_file, int32_t *array_single_data,int *size_array){
	//call the instruction based on the opcode
	if ((instruction -> opcode) <= CMP){
		uint32_t result = assemble_data_processing(instruction);
		binary_writer(dest_file,result);
	}
	else if ((instruction -> opcode) <= MLA){
		uint32_t result = assemble_multiply(instruction);
		binary_writer(dest_file,result);
	}
	else if ((instruction -> opcode) <= STR){
		uint32_t result = assemble_single_data_transfer(instruction,pc,last_address,array_single_data,size_array);
		binary_writer(dest_file,result);
	}
	else if ((instruction -> opcode) <= B){
		// if 1st operand is a label , replace it with its reference	
		uint32_t res = lookup(instruction->operand[0],symbol_table);
		if (res!=-1){
			char buffer[35];
			sprintf(buffer,"%x",res);
			strcpy(instruction->operand[0], buffer);
		}
		uint32_t result = assemble_branch(instruction,pc);
		binary_writer(dest_file,result);
	} else if ((instruction->opcode) <= INPUTN) {
		uint32_t result = assemble_special(instruction);
		binary_writer(dest_file, result);
	} else {
		fprintf(stderr, "Incorrect opcode detected\n");
		exit(EXIT_FAILURE);
	}
}

void load_assembly(char **argv){
	//create hash table structure for symbol table
	 hash_table *symbol_table = create_hash_table();

	//open the assembly file
	FILE *fp;
	fp = fopen(argv[1],"r");
	assert(fp != NULL);

	// FIRST PASS OVER SOURCE CODE

	uint32_t address = 0x0;	
	while(1){
		char buffer[MAX_LINE_SIZE];
		int res = fscanf(fp, " %[^\n]", buffer);
		if (res == EOF){
			break;
		}

		//process the buffer
		if (islabel(buffer)){
			//truncating the string before the :
			buffer[strlen(buffer)-1] = '\0';
			insert(buffer,4*address, symbol_table);
		}
		else {
			address++;
			//address = address + 0x4;
		}
	}
	fclose(fp);

	// 2ND PASS OVER SOURCE CODE
	FILE *fp2;
        fp2 = fopen(argv[1],"r");
        assert(fp2 != NULL);

	uint32_t calling_address = 0x0;

	//Creating an array of integers to write the single data expressions at the end of the assembled file
	int array_length = 100;
        int32_t *array_single_data = malloc(array_length*sizeof(int32_t));
	int size_array = 0;

	while(1){
		char buffer[MAX_LINE_SIZE];
                int res = fscanf(fp, " %[^\n]", buffer);
                if (res == EOF){
                        break;
                }

		if (!islabel(buffer)){
			//calling the tokenizer with the new string
			TokenizedInstruction *instruct = tokenize(buffer);
			//pass the tokenized instruction into the various functions
			call_instruction(instruct,symbol_table,calling_address,address,argv[2],array_single_data,&size_array);
			calling_address++;
			free_tokenized_instruction(instruct);
		}
	}
	fclose(fp2);

	//After we have processed all the instructions, writing the single data transfer expressions at the end of the assembled file
	if (size_array != 0){
		binary_writer_array(argv[2],array_single_data,size_array); 	
	}

	free(array_single_data);
	exit(EXIT_SUCCESS);
	
}



int main(int argc, char **argv) {
	if (argc != 3){
	       	printf("Incorrect no. of arguments supplied!\n");
		return EXIT_FAILURE;
	}
	load_assembly(argv);
	return 0;
}
