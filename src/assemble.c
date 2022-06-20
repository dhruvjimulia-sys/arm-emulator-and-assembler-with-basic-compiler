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
		printf("Error in writing to file");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
}


void binary_writer_array(char *dest_file, int32_t *result_array, int size){
	FILE *fp = fopen(dest_file,"a");
	assert(fp != NULL);
	
	for (int i=0; i < size; i++){
		uint32_t result = reverse(result_array[i]);
		fwrite(&result,4,1,fp);
	}

	if (ferror(fp)){
		printf("Error in writing array elements to file");
	}
	fclose(fp);
}

bool islabel(char *line){
	return line[strlen(line)-1] == ':';
}

char **allocArray (unsigned int rows, unsigned int cols){
	char** array;
	unsigned int i;

	array = (char**) malloc (sizeof(char*) * rows );
	if (!array){
		return NULL;
	}
	array[0] = (char*) malloc (rows*cols*sizeof(char*));
	if (!array[0]){
		free(array);
		return NULL;
	}
	for (i=1; i < rows; i++){
		array[i] = array[i-1] + cols;
	}
	return array;
}

void freeArray(char ** array){
	free(array[0]);
	free(array);
}

//typedef uint32_t (*func_pointer)(TokenizedInstruction);
//func_pointer array_fn_pointers = {assemble_data_processing, assemble_multiply, assemble_single_data_transfer, assemble_branch};

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
			char buffer[33];
			sprintf(buffer,"%x",res);
			strcpy(instruction->operand[0], buffer);
		}
		uint32_t result = assemble_branch(instruction,pc);
		binary_writer(dest_file,result);
	}
	else {
		printf("Incorrect opcode detected");
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
	
	

	/*
	int numlines=0;
	
	// counting the no. of new lines
	for (int c = fgetc(fp); c!= EOF; c = fgetc(fp)){
		if (c == '\n'){++numlines;}
	}

	// Creating a 2d array of strings to store the read file 
	char **data = allocArray(numlines,MAX_LINE_SIZE);
	assert (data != NULL);

	// Creating an array of integers to write the single data instructions at the end of the assembled file
        int array_length = 100;
        int32_t *array_single_data = malloc(array_length*sizeof(int32_t));
	int size_array = 0;
	

	
	//first pass over source code
	char buffer[MAX_LINE_SIZE];
	char *read = fgets(buffer,MAX_LINE_SIZE,fp);
	
	data[0] = buffer;

	uint32_t address = 0x0;
	int arrayIndex = 1;
	while (read!=NULL){
		// use the read value from the buffer
		if (islabel(buffer)){
			address = 4*address;
			// truncates the string before : character
			buffer[strlen(buffer)-2]='\0';
			insert(buffer,address, symbol_table);
	
		} else {
			address++;
		}
		data[++arrayIndex] = buffer;
		read = fgets(buffer,MAX_LINE_SIZE,fp);
	}
	
	fclose(fp);

	

	// 2nd pass, reading from the string array and passing it into the tokenizer
	uint32_t calling_address = 0x0;
	for (int i = 0; i < numlines; i++){
		if (!islabel(data[i])){
			printf("%s",data[i]);
			// call the tokenizer with data[i]
			TokenizedInstruction *instruct = tokenize(data[i]);
			// pass the tokenized structure into the various functions
			call_instruction(instruct,symbol_table,calling_address,address,argv,array_single_data,size_array);
			calling_address++;
			free_tokenized_instruction(instruct);
		}
	}

	freeArray(data);
	free_hash_table(symbol_table);

	//After we have proceesed all the instructions, writing the single data transfer instructions at the end of the assembled file
	binary_writer_array(argv[2],array_single_data,size_array,address);

	free(array_single_data);

	*/
}



int main(int argc, char **argv) {
	if (argc != 3){
	       	printf("Incorrect no. of arguments supplied!");
		return EXIT_FAILURE;
	}
	load_assembly(argv);
	return 0;
}
