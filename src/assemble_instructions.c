#include "tokenizer"
#include "assembler_utils.h"

void assemble_multiply(void) {
        uint32_t opcode;
        switch (instr->opcode) {
                case ANDEQ:
                case AND:
                        opcode = 0x0;
                        break;
                case EOR:
                        opcode = 0x1;
                        break;
                case SUB:
                        opcode = 0x2;
                        break;
                case RSB:
                        opcode = 0x3;
                        break;
                case ADD:
                        opcode = 0x4;
                        break;
                case TST:
                        opcode = 0x8;
                        break;
                case TEQ:
                        opcode = 0x9;
                        break;
                case CMP:
                        opcode = 0xa;
                        break;
                case ORR:
                        opcode = 0xc;
                        break;
                case MOV:
                        opcode = 0xd;
                        break;
                default:
                        printf("Unexpected opcode %x", instr->opcode);
                        exit(EXIT_FAILURE);
  }