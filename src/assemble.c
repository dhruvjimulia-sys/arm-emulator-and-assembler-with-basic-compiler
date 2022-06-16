#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "symbol_table.h"
#include "tokenizer.h"
#include "assemble_instructions.h"

#define MAX_LINE_SIZE 512

//printing binary instructions onto the destination file
void binary_writer(char* dest_file, uint32_t result, uint32_t address){
	FILE *fp = fopen(dest_file,"ab");
	assert(fp != NULL);
        
	// fwrite(pointer_of_writing_data, size_of_each_element_in_bytes,no_of_items_to_be_written,filepointer)
	//fwrite(&address,sizeof(address),1,fp);
	fwrite(&result,4,1,fp);
	if (ferror(fp)){
		printf("Error in writing to file");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
}


void binary_writer_array(char *dest_file, int32_t *result_array, int size,uint32_t address){
	FILE *fp = fopen(dest_file,"ab");
	assert(fp != NULL);

	//uint32_t add = address;

	for (int i=0; i < size; i++){
		//fwrite(&add,sizeof(add),1,fp);
		fwrite(&result_array[i],4,1,fp);
		//add++;
	}
	if (ferror(fp)){
		printf("Error in writing array elements to file");
	}
	fclose(fp);
}

bool islabel(char *line){
	return line[strlen(line)-2] == ':';
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
	for (i=0; i < rows; i++){
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

void call_instruction(TokenizedInstruction *instruction, hash_table *symbol_table, uint32_t pc, uint32_t last_address, char **argv, int32_t *array_single_data,int size_array){
	//call the instruction based on the opcode
	if ((instruction -> opcode) <= CMP){
		uint32_t result = assemble_data_processing(instruction);
		binary_writer(argv[2],result,pc);
	}
	else if ((instruction -> opcode) <= MLA){
		uint32_t result = assemble_multiply(instruction);
		binary_writer(argv[2],result,pc);
	}
	else if ((instruction -> opcode) <= STR){
		uint32_t result = assemble_single_data_transfer(instruction,pc,last_address,array_single_data,&size_array);
		binary_writer(argv[2],result,pc);
	}
	else if ((instruction -> opcode) <= B){
		// if 1st operand is a label , replace it with its reference	
		uint32_t res = lookup(instruction->operand[0],symbol_table);
		if (res!=-1){
			char buffer[32];
			snprintf(buffer, sizeof(buffer),"%x",res);
			instruction->operand[0] = buffer;
		}
		uint32_t result = assemble_branch(instruction,pc);
		binary_writer(argv[2],result,pc);
	}
	else {
		printf("Incorrect opcode detected");
	}
}

void load_assembly(char *filename,char **argv){
	//create hash table structure for symbol table
	hash_table *symbol_table = create_hash_table();

	//open the assembly file
	FILE *fp = fopen(filename,"r");
	assert(fp != NULL);
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
			insert(buffer,address, symbol_table->entries, symbol_table->size, symbol_table);
	
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
}



int main(int argc, char **argv) {
	if (argc != 3){
	       	printf("Incorrect no. of arguments supplied!");
		return EXIT_FAILURE;
	}
	//load_assembly(argv[1]);
	// begin the assembler loop
	return EXIT_SUCCESS;
}
