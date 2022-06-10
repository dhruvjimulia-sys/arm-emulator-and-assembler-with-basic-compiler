


void binary_writer(char* dest_file, uint32_t *binary, uint32_t cursor);

bool islabel(char *line);

char **allocArray (unsigned int rows, unsigned int cols);

void freeArray(char ** array);

void call_instruction(TokenizedInstruction *instruction, hash_table *symbol_table, uint32_t pc, uint32_t last_address);

char *load_assembly(char[] filename);


