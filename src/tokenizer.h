#include "assembler_utils.h"

TokenizedInstruction * tokenize( char * instruction );
void free_tokenized_instruction( TokenizedInstruction * tokenized );
Operation to_operation_enum( char * token );
