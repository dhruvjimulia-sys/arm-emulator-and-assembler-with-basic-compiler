#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "instructions.h"

void load(char filename[], struct Processor* processor);

bool process_instructions(uint8_t* instruction_bytes, struct Processor* processor);

void emulator_loop(struct Processor* processor);
