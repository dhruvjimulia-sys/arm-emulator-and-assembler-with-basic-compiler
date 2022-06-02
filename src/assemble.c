#include <stdlib.h>
#include <stdio.h>

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
		perror("Error in writing to file");
		exit(EXIT_FAILURE);
	}

	fclose(fp);
}


char *load_assembly(char[] filename){
	FILE *fp = fopen(filename,"r");
	assert(flp!=null);

	char buffer[MAX_LINE_SIZE];
	char *read = fgets(buffer,MAX_LINE_SIZE,fp);
	while (read!=null){
		// use the read value from the buffer
		read = fgets(buffer,MAX_LINE_SIZE,fp);
	}
	fclose(fp);
}



int main(int argc, char **argv) {
	if (argc != 3){
	       	printf("Incorrect no. of arguments supplied!");
		return EXIT_FAILURE;
	}
	read_file(argv[1]);
	// begin the assembler loop
	return EXIT_SUCCESS;
}
