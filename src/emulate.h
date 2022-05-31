#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
// "emulate.h" requires "instructions.h"

extern struct Processor processor;

void load(char filename[]);

bool condition_check(uint32_t type);

bool process_instructions(uint8_t* instruction_bytes);

void emulator_loop(uint8_t* instructions);
