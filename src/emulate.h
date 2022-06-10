#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "instructions.h"

#ifndef EMULATE_H
#define EMULATE_H

//loads instruction bytes
void load(char filename[], struct Processor* processor);

//processes different type of instructions loaded
bool process_instructions(uint8_t* instruction_bytes, struct Processor* processor);

//defines base code of the emulator loop
void emulator_loop(struct Processor* processor);

#endif
