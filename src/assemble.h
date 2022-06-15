#include <stdint.h>
#include <stdbool.h>
#include "symbol_table.h"
#include "assembler_type_definitions.h"

#ifndef ASSEMBLE_H
#define ASSEMBLE_H

//binary file writer
void binary_writer(char* dest_file, uint32_t *binary, uint32_t cursor);

//determine if line is label or instruction
bool islabel(char *line);

//allocate memory on heap given size of multi-dimensional array
char **allocArray (unsigned int rows, unsigned int cols);

//free allocated array
void freeArray(char ** array);

//calls appropriate function to assemble a specific instruction
void call_instruction(TokenizedInstruction *instruction, hash_table *symbol_table, uint32_t pc, uint32_t last_address);

//loads the lines from an assembly file
char *load_assembly(char *filename);

#endif
