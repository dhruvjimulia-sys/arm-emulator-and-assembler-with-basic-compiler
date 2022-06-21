#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "assembler_type_definitions.h"
#include "tokenizer.h"

#define MAX_NUMBER_OPERANDS 4
#define OPERAND_LENGTH 50
#define MAX_VALS_IN_BRACKETS 10
#define VAL_LENGTH 10

TokenizedInstruction* tokenize(char *instruction) {
	TokenizedInstruction *tokenized = malloc(sizeof(TokenizedInstruction));
	tokenized->operand = malloc(MAX_NUMBER_OPERANDS * sizeof(char *));
	char token[OPERAND_LENGTH];
	strcpy(token, strtok(instruction, " "));
	tokenized->opcode = to_operation_enum(token);
	uint32_t i = 0;
  char *stack[MAX_VALS_IN_BRACKETS];
  int sp = 0;
	do {
    char *token = sp == 0 ? strtok(NULL, " ,") : strtok(NULL, ",");
    if (token == NULL) {
			break;
    }
    if (token[0] == '[' || sp != 0) {
      stack[sp] = malloc(VAL_LENGTH);
      strcpy(stack[sp], token);
      sp++;
    }
    if (token[strlen(token) - 1] == ']') {
      char *realtoken = malloc(OPERAND_LENGTH);
      strcpy(realtoken, "\0");
      for (int i = 0; i < sp; i++) {
        strcat(realtoken, stack[i]);
        if (i != sp - 1) {
          strcat(realtoken, ",");
        }
        free(stack[i]);
      }
      sp = 0;
      tokenized->operand[i] = malloc(OPERAND_LENGTH);
      strcpy(tokenized->operand[i], realtoken);
      i++;
      free(realtoken);
    } else if (sp == 0) {
      tokenized->operand[i] = malloc(OPERAND_LENGTH);
      strcpy(tokenized->operand[i], token);
      i++;
    }
	} while (tokenized->operand[i - 1] != NULL);
	
	tokenized->num_operands = i;
  return tokenized;
}

void free_tokenized_instruction(TokenizedInstruction *tokenized) {
	for (uint8_t i = 0; i < tokenized->num_operands; i++) {
		free(tokenized->operand[i]);
	}
	free(tokenized->operand);
	free(tokenized);
}

Operation to_operation_enum(char *token) {
	if (!strcmp(token, "add")) {
		return ADD;
	} else if (!strcmp(token, "sub")) {
		return SUB;
	} else if (!strcmp(token, "rsb")) {
		return RSB;
	} else if (!strcmp(token, "andeq")) {
		return ANDEQ;
	} else if (!strcmp(token, "and")) {
		return AND;
	} else if (!strcmp(token, "eor")) {
		return EOR;
	} else if (!strcmp(token, "orr")) {
		return ORR;
	} else if (!strcmp(token, "lsl")) {
		return LSL;
	} else if (!strcmp(token, "mov")) {
		return MOV;
	} else if (!strcmp(token, "tst")) {
		return TST;
	} else if (!strcmp(token, "teq")) {
		return TEQ;
	} else if (!strcmp(token, "cmp")) {
		return CMP;
	} else if (!strcmp(token, "mul")) {
		return MUL;
	} else if (!strcmp(token, "mla")) {
		return MLA;
	} else if (!strcmp(token, "ldr")) {
		return LDR;
	} else if (!strcmp(token, "str")) {
		return STR;
	} else if (!strcmp(token, "beq")) {
		return BEQ;
	} else if (!strcmp(token, "bne")) {
		return BNE;
	} else if (!strcmp(token, "bge")) {
		return BGE;
	} else if (!strcmp(token, "blt")) {
		return BLT;
	} else if (!strcmp(token, "bgt")) {
		return BGT;
	} else if (!strcmp(token, "ble")) {
		return BLE;
	} else if (!strcmp(token, "b")) {
		return B;
	} else if (!strcmp(token, "prints")) {
		return PRINTS;
	} else if (!strcmp(token, "printn")) {
		return PRINTN;
	} else if (!strcmp(token, "inputs")) {
		return INPUTS;
	} else if (!strcmp(token, "inputn")) {
		return INPUTN;
	} else {
		fprintf(stderr, "Invalid opcode mnemonic: %s", token);
		exit(EXIT_FAILURE);
	}
