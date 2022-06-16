#ifndef __EMULATE_H__
#define __EMULATE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "instructions.h"

//loads instruction bytes
void load(char filename[], struct Processor* processor);

//processes different type of instructions loaded
bool process_instructions(uint8_t* instruction_bytes, struct Processor* processor);

//defines base code of the emulator loop
void emulator_loop(struct Processor* processor);

#endif
