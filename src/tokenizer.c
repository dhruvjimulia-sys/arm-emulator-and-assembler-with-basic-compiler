#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "tokenizer.h"

#define MAX_NUMBER_OPERANDS 4
#define OPERAND_LENGTH 20

TokenizedInstruction* tokenize(char *instruction) {
	TokenizedInstruction *tokenized = malloc(sizeof(TokenizedInstruction));
	tokenized->operand = malloc(MAX_NUMBER_OPERANDS * sizeof(char *));
	char token[OPERAND_LENGTH];
  strcpy(token, strtok(instruction, " "));
	tokenized->opcode = to_operation_enum(token);
	uint32_t i = 0;
	do {
    char *token = strtok(NULL, ",");
    if (token == NULL) {
      break;
    }
    tokenized->operand[i] = malloc(OPERAND_LENGTH);
    strcpy(tokenized->operand[i], token);
		i++;
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
	if (strcmp(token, "add") == 0) {
		return ADD;
	} else if (strcmp(token, "sub") == 0) {
		return SUB;
	} else if (strcmp(token, "rsb") == 0) {
		return RSB;
	} else if (strcmp(token, "andeq") == 0) {
		return ANDEQ;
	} else if (strcmp(token, "and") == 0) {
		return AND;
	} else if (strcmp(token, "eor") == 0) {
		return EOR;
	} else if (strcmp(token, "orr") == 0) {
		return ORR;
	} else if (strcmp(token, "lsl") == 0) {
		return LSL;
	} else if (strcmp(token, "mov") == 0) {
		return MOV;
	} else if (strcmp(token, "tst") == 0) {
		return TST;
	} else if (strcmp(token, "teq") == 0) {
		return TEQ;
	} else if (strcmp(token, "cmp") == 0) {
		return CMP;
	} else if (strcmp(token, "mul") == 0) {
		return MUL;
	} else if (strcmp(token, "mla") == 0) {
		return MLA;
	} else if (strcmp(token, "ldr") == 0) {
		return LDR;
	} else if (strcmp(token, "str") == 0) {
		return STR;
	} else if (strcmp(token, "beq") == 0) {
		return BEQ;
	} else if (strcmp(token, "bne") == 0) {
		return BNE;
	} else if (strcmp(token, "bge") == 0) {
		return BGE;
	} else if (strcmp(token, "blt") == 0) {
		return BLT;
	} else if (strcmp(token, "bgt") == 0) {
		return BGT;
	} else if (strcmp(token, "ble") == 0) {
		return BLE;
	}
	assert(strcmp(token, "b"));
	return B;
}
