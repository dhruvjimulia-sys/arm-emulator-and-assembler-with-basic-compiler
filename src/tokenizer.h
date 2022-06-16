#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "assembler_type_definitions.h"

//tokenize an instruction into its label, opcode and operand fields
TokenizedInstruction * tokenize( char * instruction );

//free allocated memory
void free_tokenized_instruction( TokenizedInstruction * tokenized );

//map instruction opcode into defined Operation enum (defined in assembler_utils.h)
Operation to_operation_enum( char * token );

#endif
