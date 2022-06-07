#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "symbol_table.h"

#define MAX_LINE_SIZE 512



//printing binary instructions onto the destination file
void binary_writer(char* dest_file, uint32_t *binary, uint32_t cursor){
	// should this be w or wb?
	FILE *fp = fopen(dest_file,"wb");
	assert(fp != null);
        
	// fwrite(pointer_of_writing_data, size_of_each_element_in_bytes,no_of_items_to_be_written,filepointer)
	// 3rd argument depends on how we are writing to the file - if it is in the form of an array, 3rd arg is length of array
	fwrite(binary,4,1,fp);
	if (ferrror(fp)){
		printf("Error in writing to file");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
}

hash_table *symbol_table = create_hash_table();

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

char *load_assembly(char[] filename){
	FILE *fp = fopen(filename,"r");
	assert(fp != null);
	int numlines=0;
	
	// counting the no. of new lines
	for (int c = 0; fgetc(fp); c!= EOF; c = fgetc(fp)){
		if (c == '\n'){++numlines;}
	}

	// Creating a 2d array of strings to store the read file 
	char **data = allocArray(numlines,MAX_LINE_SIZE);
	assert (data != NULL);


	//first pass over source code
	char buffer[MAX_LINE_SIZE];
	char *read = fgets(buffer,MAX_LINE_SIZE,fp);
	
	data[0] = buffer;

	uint32_t address = 0x0;
	int arrayIndex = 1;
	while (read!=null){
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
	for (int i = 0; i < numlines; i++){
		if (!isLabel(data[i])){
			// call the tokenizer with data[i]
			// pass the tokenized structure into the various functions
			//
			// TOKENIZED INSTRUCTION SHOULD REPLACE THE LABEL REFERENCES!!!!!!!!!!!!!
		}
	}

	freeArray(data);
}



int main(int argc, char **argv) {
	if (argc != 3){
	       	printf("Incorrect no. of arguments supplied!");
		return EXIT_FAILURE;
	}
	load_assembly(argv[1]);
	// begin the assembler loop
	return EXIT_SUCCESS;
}
