#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "type_definitions.h"

#ifndef EMULATE_H
#define EMULATE_H

uint8_t* load(char filename[]);

bool condition_check(uint32_t type);

bool process_instructions(uint8_t* instruction_bytes);

void emulator_loop(uint8_t* instructions);

#endif
