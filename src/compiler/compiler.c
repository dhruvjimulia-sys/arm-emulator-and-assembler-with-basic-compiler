#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include "compiler.h"
#include "symbol_table.h"

#define MEM_SIZE 65536
#define MAX_INNER_WHILE_LOOPS 100
#define MAX_LINE_LENGTH_BAS 251
#define MAX_LINE_LENGTH_ASM MAX_LINE_LENGTH_BAS 
#define MAX_STRING_LITERAL_LENGTH 244
#define INTEGER_NUM_BYTES 4

#define EFFECTIVE_MEM_SIZE (MEM_SIZE - MAX_STRING_LITERAL_LENGTH - INTEGER_NUM_BYTES)
#define MAX_STACK_SIZE EFFECTIVE_MEM_SIZE / 8
#define VAR_STRING_STACK_SIZE 3 * (EFFECTIVE_MEM_SIZE / 8)
#define ARRAY_STACK_SIZE 3 * (EFFECTIVE_MEM_SIZE / 8)
#define MAX_NUM_LINES_ASM EFFECTIVE_MEM_SIZE / 8

#define MAX_NUM_LINES_BAS MAX_NUM_LINES_ASM

#define SEED_MEM_ADDRESS MEM_SIZE - INTEGER_NUM_BYTES
#define STRING_START_MEM EFFECTIVE_MEM_SIZE
#define STACK_START_MEM EFFECTIVE_MEM_SIZE - 4
#define VAR_STRING_START_MEM (EFFECTIVE_MEM_SIZE - MAX_STACK_SIZE) - VAR_STRING_STACK_SIZE
#define ARRAY_START_MEM VAR_STRING_START_MEM - ARRAY_STACK_SIZE

#define EQS(s1, s2) !strcmp(s1, s2)
#define STACK_POINTER 13
#define PC_REGISTER 15
#define NUM_GP_REGISTERS 12

#define OPERATORS "+-*"
#define COMP_OPERATORS { "<>", ">=", "<=", "=", "<", ">" }
#define NUM_COMP_OPERATORS 6
#define FILE_NAME_LENGTH 50

char **read_file(char *filename, int *basic_num_lines) {
  char **file = malloc(MAX_NUM_LINES_BAS * sizeof(char *));
  FILE *fp = fopen(filename, "r"); 
  assert(fp);
  char *line;
  int i = 0;
  do {
    line = malloc(MAX_LINE_LENGTH_BAS);
    fgets(line, MAX_LINE_LENGTH_BAS, fp);
    file[i] = line;
    printf("%s", file[i]);
    i++;
  } while (!EQS(line, "END\n") && !EQS(line, "END"));
  *basic_num_lines = i;
  fclose(fp);
  return file;
}

void write_file(char **file, char *filename, int asm_num_lines) {
  FILE *fp = fopen(filename, "w");
  assert(fp);
  for (int i = 0; i < asm_num_lines; i++) {
    fprintf(fp, "%s", file[i]);
  }
  fclose(fp);
}

void free_file(char **file, int num_lines) {
  for (int i = 0; i < num_lines; i++) {
    free(file[i]);
  }
  free(file);
}

bool file_contains(char **basic_file, char *string, int basic_num_lines) {
  for (int i = 0; i < basic_num_lines; i++) {
    if (strcmp(basic_file[i], string) == 0) {
      return true;
    }
  }
  return false;
}

bool file_contains_command(char **basic_file, char* string, int basic_num_lines) {
  char entire[MAX_LINE_LENGTH_BAS];
  for (int i = 0; i < basic_num_lines; i++) {
    strcpy(entire, basic_file[i]);
    if (EQS(strtok(entire, " "), string)) {
      return true;
    }
  }
  return false;
}

bool file_contains_input_string(char **basic_file, int basic_num_lines) {
  char entire[MAX_LINE_LENGTH_BAS];
  for (int i = 0; i < basic_num_lines; i++) {
    strcpy(entire, basic_file[i]);
    char *command = strtok(entire, " ");
    if (EQS(command, "INPUT")) {
      char *variable = strtok(NULL, "\n");
      if (variable[strlen(variable) - 1] == '$') {
        return true;
      }
    }
  }
  return false;
}

bool file_contains_rnd(char **basic_file, int basic_num_lines) {
  for (int i = 0; i < basic_num_lines; i++) {
    if (is_substring_of("RND", basic_file[i]) != -1) {
      return true;
    }
  }
  return false;
}

int first_free_reg(bool *free_reg_arr) {
  for (int i = 0; i < NUM_GP_REGISTERS; i++) {
    if (free_reg_arr[i]) {
      return i;
    }
  }
  return -1;
}

bool is_val_part_of(char val, char *contains) {
  for (int j = 0; j < strlen(contains); j++) {
    if (val == contains[j]) {
      return true;
    }
  }
  return false;
}

bool str_contains_any_of(char *str, char *contains) {
  for (int i = 0; i < strlen(str); i++) {
    if (is_val_part_of(str[i], contains)) {
      return true;
    }
  }
  return false;
}

bool in_list_of_strings(char *str, char **list, int num_elems) {
  for (int i = 0; i < num_elems; i++) {
    if (EQS(list[i], str)) {
      return true;
    }
  }
  return false;
}

int is_substring_of(char *sub, char *str) {
  for (int i = 0; i < strlen(str) - strlen(sub) + 1; i++) {
    bool substring = true;
    for (int j = 0; j < strlen(sub); j++) {
      if (str[i + j] != sub[j]) {
        substring = false;
      }
    }
    if (substring) {
      return i;
    }
  }
  return -1;
}

char *is_any_substring_of(char **subs, char *str, int *index) {
  for (int i = 0; i < NUM_COMP_OPERATORS; i++) {
    int pos = is_substring_of(subs[i], str);
    if (pos != -1) {
      *index = pos;
      return subs[i];
    }
  }
  return NULL;
}

void init_op_precedence(hash_table *op_precedence) {
  insert("+", 1, op_precedence);
  insert("-", 1, op_precedence);
  insert("*", 2, op_precedence);
}

bool is_string(char *expression) {
  assert(expression[0]);
  return expression[0] == '\"' || expression[strlen(expression) - 1] == '$';
}

bool is_string_variable(char *expression) {
  return expression[strlen(expression) - 1] == '$';
}

bool is_string_literal(char *expression) {
  return expression[0] == '\"';
}

char *shunting_yard(char *expression, hash_table *op_precedence) {
  char *operands[MAX_LINE_LENGTH_ASM];
  char operators[MAX_LINE_LENGTH_ASM];
  int operand_index = 0;
  int operator_index = 0;
  for (int i = 0; i < strlen(expression); i++) {
    if (expression[i] == '(') {
      operators[operator_index] = '(';
      operator_index++;
    } else if (expression[i] == ')') {
      if (operator_index == 0) {
        fprintf(stderr, "Compiler error: Mismatched parentheses\n");
        exit(EXIT_FAILURE);
      }
      while (operators[operator_index - 1] != '(') {
        if (operand_index < 2) {
          fprintf(stderr, "Compiler error: Error parsing arithmetic expression\n");
          exit(EXIT_FAILURE);
        }
        char *first = operands[operand_index - 1];
        char *second = operands[operand_index - 2]; 
        operand_index -= 2;
        char *merged = malloc(MAX_LINE_LENGTH_ASM / 2);
        sprintf(merged, "(%s%c%s)", second, operators[operator_index - 1], first);
        free(first);
        free(second);
        operands[operand_index] = merged;
        operator_index--;
        operand_index++;
        if (operator_index == 0) {
          fprintf(stderr, "Compiler error: Mismatched parentheses\n");
          exit(EXIT_FAILURE);
        }
      }
      operators[operator_index - 1] = '\0';
      operator_index--;
    } else if (is_val_part_of(expression[i], OPERATORS)) {
      char o1[2];
      char o2[2];
      if (operator_index != 0) {
        sprintf(o1, "%c", expression[i]);
        sprintf(o2, "%c", operators[operator_index - 1]);
      }
      while (operator_index != 0 && operators[operator_index - 1] != '(' && (lookup(o1, op_precedence) <= lookup(o2, op_precedence))) {
        char operator = operators[operator_index - 1];
        if (operand_index < 2) {
          fprintf(stderr, "Compiler error: Error parsing arithmetic expression\n");
          exit(EXIT_FAILURE);
        }
        char *first = operands[operand_index - 1];
        char *second = operands[operand_index - 2]; 
        operand_index -= 2;
        char *merged = malloc(MAX_LINE_LENGTH_ASM / 2);
        sprintf(merged, "(%s%c%s)", second, operator, first);
        free(first);
        free(second);
        operands[operand_index] = merged;
        operator_index--;
        operand_index++;
      }
      operators[operator_index] = expression[i];
      operator_index++;
    } else {
      char *token = malloc(MAX_LINE_LENGTH_ASM / 2);
      int j = 0;
      token[0] = expression[i];
      while (i + 1 < strlen(expression) && !is_val_part_of(expression[i + 1], OPERATORS) && expression[i + 1] != ')' && expression[i + 1] != '(') {
        j++; 
        token[j] = expression[i + 1];
        token[j + 1] = '\0';
        i++;
        if (EQS(token, "LEN") || EQS(token, "RND")) {
          j++;
          i++;
          while (expression[i] != ')') {
            token[j] = expression[i];
            j++;
            i++;
          }
          token[j] = expression[i];
          token[j + 1] = '\0';
          break;
        }
      }
      token[j + 1] = '\0';
      operands[operand_index] = token;
      operand_index++;
    }
  }
  while (operator_index > 0) {
    if (operators[operator_index - 1] == '(') {
      fprintf(stderr, "Compiler error: Mismatched parentheses\n");
      exit(EXIT_FAILURE);
    }
    if (operand_index < 2) {
      fprintf(stderr, "Compiler error: Error parsing arithmetic expression\n");
      exit(EXIT_FAILURE);
    }
    char *first = operands[operand_index - 1];
    char *second = operands[operand_index - 2]; 
    operand_index -= 2;
    char *merged = malloc(MAX_LINE_LENGTH_ASM / 2);
    sprintf(merged, "(%s%c%s)", second, operators[operator_index - 1], first);
    free(first);
    free(second);
    operands[operand_index] = merged;
    operator_index--;
    operand_index++;
  }
  assert(operands[0]);
  return operands[0];
}

char *eval_arithmetic(char *expression, int *asm_num_lines, char **assembly_file, hash_table *var_reg_table, bool *free_reg_arr, hash_table *op_precedence) {
  char *final_expression = malloc(MAX_LINE_LENGTH_ASM);
  char expression_copy[MAX_LINE_LENGTH_ASM];
  static int num_len = 0;
  static int num_rnd_branches = 0;
  strcpy(expression_copy, expression);
  char *function = strtok(expression_copy, "("); 
  if (EQS(expression, "0\n") || atoi(expression) != 0) {
    final_expression[0] = '#';
    final_expression[1] = '\0';
    strtok(expression, "\n");
    strcat(final_expression, expression);
    return final_expression;
  }
  int reg = lookup(strtok(expression, "\n"), var_reg_table);
  if (reg != -1) {
    char strreg[MAX_LINE_LENGTH_ASM];
    final_expression[0] = 'r';
    final_expression[1] = '\0';
    sprintf(strreg, "%d", reg);
    strcat(final_expression, strreg);
    return final_expression;
  } else if (str_contains_any_of(expression, OPERATORS)) {
    assert(expression[0] == '(');
    assert(expression[strlen(expression) - 1] == ')');
    int precedence = 0;
    for (int i = 1; i < strlen(expression) - 1; i++) {
      if (expression[i] == '(') {
        precedence++;
      } else if (expression[i] == ')') {
        precedence--;
      } else if (is_val_part_of(expression[i], OPERATORS) && precedence == 0) {
        char first[strlen(expression)];
        char second[strlen(expression)];
        memcpy(first, expression + 1, i - 1);
        first[i - 1] = '\0';
        memcpy(second, expression + i + 1, strlen(expression) - i - 2);
        second[strlen(expression) - i - 2] = '\0';
        char *first_final = eval_arithmetic(first, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, op_precedence);
        if (first_final[0] == 'r') {
          free_reg_arr[atoi(first_final + 1)] = false;
        }
        char *second_final = eval_arithmetic(second, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, op_precedence);
        int final_reg = first_free_reg(free_reg_arr);
        sprintf(final_expression, "r%d", final_reg);
        free_reg_arr[final_reg] = false;
        switch (expression[i]) {
          case '+':
            sprintf(assembly_file[*asm_num_lines], "add %s,%s,%s\n", final_expression, first_final, second_final);
            break;
          case '-':
            sprintf(assembly_file[*asm_num_lines], "sub %s,%s,%s\n", final_expression, first_final, second_final);
            break;
          case '*':
            sprintf(assembly_file[*asm_num_lines], "mul %s,%s,%s\n", final_expression, first_final, second_final);
            break;
        }
        *asm_num_lines = *asm_num_lines + 1;
        assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM); 
        free_reg_arr[atoi(first_final + 1)] = true;
        free(first_final);
        free(second_final);
        free_reg_arr[final_reg] = true;
        return final_expression;
      }
    }
  } else if (EQS(function, "LEN")) {
    strcpy(expression_copy, expression);
    function = strtok(expression_copy, "(");
    char *variable = strtok(NULL, ")");
    int reg_var = 0;
    if (variable[strlen(variable) - 1] == '$') {
      reg_var = lookup(variable, var_reg_table);
    } else {
      assert(variable[0] == '\"');
      store_string_literal_in_memory(variable, free_reg_arr, asm_num_lines, assembly_file, STRING_START_MEM);
      *asm_num_lines = *asm_num_lines + 1;
      assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    }

    if (reg_var == -1) {
      fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", variable);
      exit(EXIT_FAILURE);
    }
    int i = first_free_reg(free_reg_arr);
    free_reg_arr[i] = false;
    int val = first_free_reg(free_reg_arr);

    sprintf(assembly_file[*asm_num_lines], "mov r%d,#0\n", i);
    
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "b .LEN%d\n", num_len * 2);

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], ".LEN%d\n", num_len * 2 + 1);

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "add r%d,r%d,#1\n", i, i);
    
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], ".LEN%d:\n", num_len * 2);

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    if (variable[0] == '$') {
      sprintf(assembly_file[*asm_num_lines], "ldr r%d,[r%d,r%d]\n", val, reg_var, i);
    } else {
      sprintf(assembly_file[*asm_num_lines], "ldr r%d,[r%d,#0x%x]\n", val, i, STRING_START_MEM);
    }

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "cmp r%d,#0\n", val);

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "bne .LEN%d\n", num_len * 2 + 1);

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(expression, "r%d", i);
    num_len++;
    free_reg_arr[i] = true;
  } else if (EQS(function, "RND")) {
    strcpy(expression_copy, expression);
    function = strtok(expression_copy, "(");
    char *subexpression = strtok(NULL, ")");
    int arg = atoi(subexpression);
    if (arg == 0) {
      fprintf(stderr, "Compiler error: Argument to RND not non-zero integer\n");
      exit(EXIT_FAILURE);
    }
    int mult = (rand() % arg) + 1;
    int inc = rand() % arg;
    int seed_reg = first_free_reg(free_reg_arr);

    sprintf(assembly_file[*asm_num_lines], "ldr r%d,#%d\n", seed_reg, SEED_MEM_ADDRESS);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "mul r%d.r%d,#%d\n", seed_reg, seed_reg, mult);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "add r%d,r%d,#%d\n", seed_reg, seed_reg, inc);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "b .RND%d:\n", num_rnd_branches * 2);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], ".RND%d:\n", num_rnd_branches * 2 + 1);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "sub r%d,r%d,#%d\n", seed_reg, seed_reg, arg);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], ".RND%d:\n", num_rnd_branches * 2);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "cmp r%d,#%d\n", seed_reg, arg);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "bge .RND%d\n", num_rnd_branches * 2 + 1);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "str r%d,#%d\n", seed_reg, SEED_MEM_ADDRESS);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(expression, "r%d", seed_reg);
    num_rnd_branches++;
  } else {
    fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", strtok(expression, "\n"));
    exit(EXIT_FAILURE);
  }
  strcpy(final_expression, expression);
  return final_expression;
}

void eval_conditional(char *cond, int *asm_num_lines, char **assembly_file, hash_table *var_reg_table, bool *free_reg_arr, int *branch_num, char *basic_command, char **labels, int num_labels, hash_table *op_precedence) {
  static int string_eq_branch_num = 0;
  int *index = malloc(sizeof(int));
  char *comp_op[NUM_COMP_OPERATORS] = COMP_OPERATORS;
  char *operator = is_any_substring_of(comp_op, cond, index);
  if (!operator) {
    fprintf(stderr, "Compiler error: comparative operator not found\n");
    exit(EXIT_FAILURE);
  }
  char first[MAX_LINE_LENGTH_ASM];
  char second[MAX_LINE_LENGTH_ASM];
  memcpy(first, cond, *index);
  first[*index] = '\0';
  memcpy(second, cond + *index + strlen(operator), strlen(cond) - *index - 1);
  second[strlen(cond) - *index - 1] = '\0';
  if (is_string(first) ^ is_string(second)) {
    fprintf(stderr, "Compiler error: incorrect comparison string with number\n");
    exit(EXIT_FAILURE);
  }
  if (is_string(first) && is_string(second)) {
    int rega = 0;
    int regb = 0;
    if (!EQS(operator, "=")) {
      fprintf(stderr, "Compiler error: comparative operator not identified for strings\n");
      exit(EXIT_FAILURE);
    }
    if (is_string_literal(first) && is_string_literal(second)) {
      fprintf(stderr, "Compiler error: comparison of string literals\n");
      exit(EXIT_FAILURE);
    } else if (is_string_literal(first) && is_string_variable(second)) {
      rega = first_free_reg(free_reg_arr);
      free_reg_arr[rega] = false;
      regb = lookup(second, var_reg_table);
      if (regb == -1) {
        fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", second);
        exit(EXIT_FAILURE);
      } 
      sprintf(assembly_file[*asm_num_lines], "mov r%d,#%d", rega, STRING_START_MEM);
      *asm_num_lines = *asm_num_lines + 1;
      assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    } else if (is_string_literal(second) && is_string_variable(first)) {
      rega = lookup(first, var_reg_table);
      if (rega == -1) {
        fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", first);
        exit(EXIT_FAILURE);
      }
      regb = first_free_reg(free_reg_arr);
      free_reg_arr[regb] = false;
    } else {
      assert(is_string_variable(first));
      assert(is_string_variable(second));
      rega = lookup(first, var_reg_table);
      if (rega == -1) {
        fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", first);
        exit(EXIT_FAILURE);
      }
      regb = lookup(second, var_reg_table);
      if (regb == -1) {
        fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", second);
        exit(EXIT_FAILURE);
      }
    }

    int counter = first_free_reg(free_reg_arr);
    free_reg_arr[counter] = false;
    int aval = first_free_reg(free_reg_arr);
    free_reg_arr[aval] = false;
    int bval = first_free_reg(free_reg_arr);
    free_reg_arr[bval] = false;

    sprintf(assembly_file[*asm_num_lines], "mov r%d,#0\n", counter);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "b .STREQ%d\n", string_eq_branch_num * 4);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], ".STREQ%d\n", string_eq_branch_num * 4 + 1);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "add r%d,r%d,#1\n", counter, counter);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], ".STREQ%d:\n", string_eq_branch_num * 4);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);

    sprintf(assembly_file[*asm_num_lines], "ldr r%d,[r%d,r%d]\n", aval, rega, counter);
     *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);

    sprintf(assembly_file[*asm_num_lines], "ldr r%d,[r%d,r%d]\n", bval, regb, counter);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);

    sprintf(assembly_file[*asm_num_lines], "cmp r%d,#0\n", aval);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "beq .STREQ%d\n", string_eq_branch_num * 4 + 2);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "cmp r%d,r%d\n", aval, bval);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "beq .STREQ%d\n", string_eq_branch_num * 4 + 1);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "b .STREQ%d\n", string_eq_branch_num * 4 + 3);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], ".STREQ%d:\n", string_eq_branch_num * 4 + 2);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "cmp r%d,#0\n", bval);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "bne .STREQ%d\n", string_eq_branch_num * 4 + 3);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    int *sub_asm_num_lines = malloc(sizeof(int *));
    *sub_asm_num_lines = 0;
    char **assembly = compile_instructions(&basic_command, 1, sub_asm_num_lines, *branch_num, labels, num_labels);
    for (int i = 0; i < *sub_asm_num_lines; i++) { 
      sprintf(assembly_file[*asm_num_lines], "%s", assembly[i]);
      *asm_num_lines = *asm_num_lines + 1;
      assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);
    }
    free_file(assembly, *sub_asm_num_lines);
    free(sub_asm_num_lines);
    
    sprintf(assembly_file[*asm_num_lines], ".STREQ%d:\n", string_eq_branch_num * 4 + 3); 
    string_eq_branch_num++;
    free_reg_arr[aval] = true;
    free_reg_arr[bval] = true;
    free_reg_arr[counter] = true;
  } else if (!is_string(first) && !is_string(second)) {
    char *bracketed_first = shunting_yard(first, op_precedence);
    char *final_first = eval_arithmetic(bracketed_first, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, op_precedence);
    free(bracketed_first);
    free_reg_arr[atoi(final_first + 1)] = false;

    char *bracketed_second = shunting_yard(second, op_precedence);
    char *final_second = eval_arithmetic(bracketed_second, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, op_precedence);
    free(bracketed_second);
    sprintf(assembly_file[*asm_num_lines], "cmp %s,%s\n", final_second, final_first);
    free_reg_arr[atoi(final_first + 1)] = true;
    char cond_code[3];
    if (EQS(operator, ">")) {
      strcpy(cond_code, "gt");
    } else if (EQS(operator, "<")) {
      strcpy(cond_code, "lt");
    } else if (EQS(operator, "=")) {
      strcpy(cond_code, "eq");
    } else if (EQS(operator, "<>")) {
      strcpy(cond_code, "ne");
    } else if (EQS(operator, "<=")) {
      strcpy(cond_code, "le");
    } else if (EQS(operator, ">=")) {
      strcpy(cond_code, "ge");
    } else {
      assert(false);
    }

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "b%s .L%d\n", cond_code, *branch_num);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], "b .L%d\n", *branch_num + 1);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

    sprintf(assembly_file[*asm_num_lines], ".L%d:\n", *branch_num);
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);

    int prev_branch_num = *branch_num + 1;
    int *sub_asm_num_lines = malloc(sizeof(int *));
    *sub_asm_num_lines = 0;
    char **assembly = compile_instructions(&basic_command, 1, sub_asm_num_lines, *branch_num, labels, num_labels);
    for (int i = 0; i < *sub_asm_num_lines; i++) { 
      sprintf(assembly_file[*asm_num_lines], "%s", assembly[i]);
      *asm_num_lines = *asm_num_lines + 1;
      assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);
    }

    free_file(assembly, *sub_asm_num_lines);
    sprintf(assembly_file[*asm_num_lines], ".L%d:\n", prev_branch_num);
    free(sub_asm_num_lines);
    *branch_num = *branch_num + 2;
    free(final_first);
    free(final_second);
  }
  free(index);
}

void replace_on_with_if(char **basic_file, int basic_num_lines) {
  for (int i = 0; i < basic_num_lines; i++) {
    int pos = is_substring_of("ON", basic_file[i]);
    if (pos != -1) {
      basic_file[i][pos] = 'I';
      basic_file[i][pos + 1] = 'F';
    }
  }
}

void get_binary(uint8_t byte, uint8_t *binary) {
    int i = CHAR_BIT;
    int j = 0;
    while(i--) {
        binary[j] = '0' + ((byte >> i) & 1);
        j++;
    }
    binary[j] = '\0';
}

char *strip_char_from_string(char *str, char val) {
  char *stripped = malloc(strlen(str) + 1);
  int j = 0;
  for (int i = 0; i < strlen(str); i++) {
    if (str[i] != val) {
      stripped[j] = str[i];
      j++;
    }
  }
  stripped[j] = '\0';
  return stripped;
}

int store_string_literal_in_memory(char *literal, bool *free_reg_arr, int *asm_num_lines, char **assembly_file, int start_mem) {
  if (literal[strlen(literal) - 1] != '\"' || literal[0] != '\"') {
    fprintf(stderr, "Compiler error: String literal parse error: \" missing\n");
    exit(EXIT_FAILURE);
  }
  literal[strlen(literal) - 1] = '\0';

  char *str_without_esc = strip_char_from_string(literal, '\\');
  int num_ins = (strlen(str_without_esc + 1) + 4) / 4;
  free(str_without_esc);
  
  int reg = first_free_reg(free_reg_arr);
  free_reg_arr[reg] = false;
  int addr_reg = first_free_reg(free_reg_arr);
  int h = 0;
  for (int i = 0; i < num_ins; i++) {
    // uint32_t strval = 0;
    if (i != 0) {
      *asm_num_lines = *asm_num_lines + 1;
      assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    }
    sprintf(assembly_file[*asm_num_lines], "mov r%d,#0\n", reg);

    for (int j = 0; j < 4; j++) {
      if (i * 4 + h + j + 1 < strlen(literal)) {
        *asm_num_lines = *asm_num_lines + 1;
        assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

        if (literal[i * 4 + h + j + 1] == '\\') {
          if (literal[i * 4 + h + j + 2] == 'n') {
            // strval += '\n';
            h++;
            sprintf(assembly_file[*asm_num_lines], "add r%d,r%d,#%d\n", reg, reg, '\n');
          } else if (literal[i * 4 + h + j + 2] == '\\') {
            h++;
            sprintf(assembly_file[*asm_num_lines], "add r%d,r%d,#%d\n", reg, reg, '\\');
          } else {
            fprintf(stderr, "Compiler error: string escape character parsing error\n");
            exit(EXIT_FAILURE);
          }
        } else { 
          // strval += literal[i * 4 + h + j + 1];
          sprintf(assembly_file[*asm_num_lines], "add r%d,r%d,#%d\n", reg, reg, literal[i * 4 + h + j + 1]);
        }
      }
      if (j != 3) {
        *asm_num_lines = *asm_num_lines + 1;
        assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);
        sprintf(assembly_file[*asm_num_lines], "lsl r%d,#8\n", reg);
        // strval <<= 8;
      }
    }
    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "mov r%d,#%d\n", addr_reg, start_mem + 4 * i);

    *asm_num_lines = *asm_num_lines + 1;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "str r%d,[r%d]\n", reg, addr_reg);
  }
  free_reg_arr[reg] = true;
  return num_ins;
}

char **compile_instructions(char **basic_file, int basic_num_lines, int *asm_num_lines, int branch_num, char **labels, int num_labels) {
  char **assembly_file = malloc(MAX_NUM_LINES_ASM * sizeof(char *));
  hash_table *op_precedence = create_hash_table();
  init_op_precedence(op_precedence);
  int inp_branch_num = 0;
  bool free_reg_arr[NUM_GP_REGISTERS];
  int string_mem_index = 0;

  for (int i = 0; i < NUM_GP_REGISTERS; i++) {
    free_reg_arr[i] = true;
  }
  hash_table *var_reg_table = create_hash_table();
  *asm_num_lines = 0;

  if (file_contains_command(basic_file, "GOSUB", basic_num_lines)) {
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "mov r%d,#0x%x\n", STACK_POINTER, STACK_START_MEM);
    *asm_num_lines = *asm_num_lines + 1;
  }

  if (file_contains_input_string(basic_file, basic_num_lines)) {
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "mov r14,#%d\n", VAR_STRING_START_MEM);
    *asm_num_lines = *asm_num_lines + 1;
  }

  if (file_contains_rnd(basic_file, basic_num_lines)) {
    int free_reg = first_free_reg(free_reg_arr);
    int seed = rand() % 10;
    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "mov r%d,#%d\n", free_reg, seed);
    *asm_num_lines = *asm_num_lines + 1;

    assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
    sprintf(assembly_file[*asm_num_lines], "str r%d,#%d\n", free_reg, SEED_MEM_ADDRESS);
    *asm_num_lines = *asm_num_lines + 1;
  }

  for (int i = 0; i < basic_num_lines; i++) {
    char entire[MAX_LINE_LENGTH_BAS];
    strcpy(entire, basic_file[i]);
    char *command = strtok(entire, " ");
    if (!EQS(command, "REM")) { 
      assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
      if (EQS(command, "GOSUB")) {
        sprintf(assembly_file[*asm_num_lines], "str r%d,[r%d],#-4\n", PC_REGISTER, STACK_POINTER);
        *asm_num_lines = *asm_num_lines + 1;
        assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
      }
      if (EQS(command, "GOTO") || EQS(command, "GOSUB")) {
        char output[MAX_LINE_LENGTH_ASM] = "b ";
        char *label = strtok(NULL, " ");
        int len = strlen(label);
        label[len - 1] = ':';
        label[len] = '\n';
        label[len + 1] = '\0';
        if (in_list_of_strings(label, labels, num_labels)) {
          label[len - 1] = '\n';
          label[len] = '\0';
          strcat(output, label);
          strcpy(assembly_file[*asm_num_lines], output);
        } else {
          label[len - 1] = '\0';
          fprintf(stderr, "Compiler error: Label \"%s\" not found\n", label);
          exit(EXIT_FAILURE);
        }
      } else if (EQS(command, "END\n") || EQS(command, "BREAK\n") || EQS(command, "END")) {
        char halt[MAX_LINE_LENGTH_ASM];
        strcpy(halt, "andeq r0,r0,r0\n");
        memcpy(assembly_file[*asm_num_lines], halt, MAX_LINE_LENGTH_ASM);
      } else if (EQS(command, "RETURN\n")) {
        int reg = first_free_reg(free_reg_arr);

        sprintf(assembly_file[*asm_num_lines], "ldr r%d,[r%d]\n", reg, STACK_POINTER);
        *asm_num_lines = *asm_num_lines + 1;
        assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

        sprintf(assembly_file[*asm_num_lines], "add r%d,#4\n", reg);
        *asm_num_lines = *asm_num_lines + 1;
        assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);

        sprintf(assembly_file[*asm_num_lines], "b r%d\n", reg);
      } else if (EQS(command, "LET")) {
        char *eq = strtok(NULL, "\n");
        char *variable = strtok(eq, "=");
        char *expression = strtok(NULL, "=");
        if (expression == NULL || EQS(expression, "\n")) {
          fprintf(stderr, "%s\n", "Compiler error: Incorrect syntax for LET statement");
          exit(EXIT_FAILURE);
        }
        uint32_t reg = lookup(variable, var_reg_table);
        if (reg == -1) {
          reg = first_free_reg(free_reg_arr);
          insert(variable, reg, var_reg_table);
          free_reg_arr[reg] = false;
        }
        if (variable[strlen(variable) - 1] != '$') { 
          expression = shunting_yard(strtok(expression, "\n"), op_precedence);
          strcat(expression, "\n");
          char *final_exp = eval_arithmetic(expression, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, op_precedence); 
          sprintf(assembly_file[*asm_num_lines], "mov r%d,%s\n", reg, final_exp);
          free(final_exp);
          free(expression);
        } else {
          int string_mem_index_diff = store_string_literal_in_memory(strtok(expression, "\n"), free_reg_arr, asm_num_lines, assembly_file, VAR_STRING_START_MEM + string_mem_index);
          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "ldr r%d,=0x%x\n", reg, VAR_STRING_START_MEM + string_mem_index);
          string_mem_index += string_mem_index_diff * 4;
        }
      } else if (EQS(command, "IF")) {
        char *cond = strtok(NULL, " ");
        char *token = strtok(NULL, " ");
        if (EQS(token, "THEN")) {
          token = strtok(NULL, " ");
        }
        int pos = is_substring_of(token, basic_file[i]);
        char basic_command[MAX_LINE_LENGTH_ASM];
        strcpy(basic_command, basic_file[i] + pos);
        eval_conditional(cond, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, &branch_num, basic_command, labels, num_labels, op_precedence);
      } else if (EQS(command, "PRINT")) {
        char str[MAX_NUM_LINES_ASM];
        strcpy(str, strtok(NULL, "\n"));

        if (str[0] == '\"') {
          store_string_literal_in_memory(str, free_reg_arr, asm_num_lines, assembly_file, STRING_START_MEM);
          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "prints #%d\n", STRING_START_MEM);
        } else if (str[strlen(str) - 1] == '$') {
          uint32_t string_reg = lookup(str, var_reg_table);
          if (string_reg != -1) {
            sprintf(assembly_file[*asm_num_lines], "prints r%d\n", string_reg);
          } else {
            fprintf(stderr, "Compiler error: variable \"%s\" not declared\n", str);
            exit(EXIT_FAILURE);
          }
        } else {
          char *bracketed_str = shunting_yard(str, op_precedence);
          char *print_expr = eval_arithmetic(bracketed_str, asm_num_lines, assembly_file, var_reg_table, free_reg_arr, op_precedence);
          if (print_expr[0] == '#') {
            int free_reg = first_free_reg(free_reg_arr);
            sprintf(assembly_file[*asm_num_lines], "mov r%d,%s\n", free_reg, print_expr);
            *asm_num_lines = *asm_num_lines + 1;
            assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
            sprintf(assembly_file[*asm_num_lines], "printn r%d\n", free_reg);
          } else {
            sprintf(assembly_file[*asm_num_lines], "printn %s\n", print_expr);
          }
          free(print_expr);
          free(bracketed_str);
        }
      } else if (EQS(command, "INPUT")) {
        char var[MAX_NUM_LINES_ASM];
        strcpy(var, strtok(NULL, "\n"));
        int reg = lookup(var, var_reg_table);
        if (reg == -1) {
          int new_reg = first_free_reg(free_reg_arr);
          insert(var, new_reg, var_reg_table);
          free_reg_arr[new_reg] = false;
          reg = new_reg;
        }

        if (var[strlen(var) - 1] == '$') {
          int val = first_free_reg(free_reg_arr);
          sprintf(assembly_file[*asm_num_lines], "inputs r14\n");

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "mov r%d,r14\n", reg);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "b .INP%d\n", inp_branch_num * 2);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], ".INP%d:\n", inp_branch_num * 2 + 1);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "add r14,r14,#1\n");

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], ".INP%d:\n", inp_branch_num * 2);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "ldr r%d,[r14]\n", val);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "cmp r%d,#0\n", val);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_LINE_LENGTH_ASM);
          sprintf(assembly_file[*asm_num_lines], "bne .INP%d\n", inp_branch_num * 2 + 1);

          *asm_num_lines = *asm_num_lines + 1;
          assembly_file[*asm_num_lines] = malloc(MAX_NUM_LINES_ASM);
          sprintf(assembly_file[*asm_num_lines], "add r14,r14,#2\n");

          inp_branch_num++;
        } else {
          sprintf(assembly_file[*asm_num_lines], "inputn r%d\n", reg);
        }
      } else {
        strcpy(assembly_file[*asm_num_lines], basic_file[i]);
      }
      *asm_num_lines = *asm_num_lines + 1;
    }
  }
  free_hash_table(var_reg_table);
  free_hash_table(op_precedence);
  return assembly_file;
}

int get_labels(char **labels, char **basic_file, int basic_num_lines) {
  int num_labels = 0;
  for (int i = 0; i < basic_num_lines; i++) {
    if (basic_file[i][strlen(basic_file[i]) - 1]) {
      labels[num_labels] = basic_file[i]; 
      num_labels++;
    }
  }
  return num_labels;
}

void replace_do_loop_with_if(char **basic_file, int *basic_num_lines, int *num_generated_labels) {
  int start_val = *num_generated_labels;
  int diff = 0;
  char entire[MAX_LINE_LENGTH_ASM];
  for (int i = 0; i < *basic_num_lines; i++) {
    strcpy(entire, basic_file[i]);
    if (EQS(basic_file[i], "DO\n")) {
      sprintf(basic_file[i], ".L%d:\n", start_val + diff);
      diff++;
      if (start_val + diff > *num_generated_labels) {
        *num_generated_labels = start_val + diff;
      }
    } else if (EQS(strtok(entire, " "), "LOOP")) {
      char *cond = strtok(NULL, "\n");
      diff--;
      sprintf(basic_file[i], "IF %s THEN GOTO .L%d\n", cond, start_val + diff);
    }
  }
  if (diff != 0) {
    fprintf(stderr, "Compiler error: missing DO or LOOP in DO..LOOP block\n");
    exit(EXIT_FAILURE);
  }
}

char **replace_while_wend_with_if(char **basic_file, int *basic_num_lines, int *num_generated_labels) {
  int start_val = *num_generated_labels;
  int new_basic_num_lines = -1;
  char **cond_stack = malloc(MAX_INNER_WHILE_LOOPS);
  char **new_basic_file = malloc(MAX_NUM_LINES_BAS * sizeof(char *));
  static const int MAX_COND_LENGTH = 10;

  int diff = 0;
  char entire[MAX_LINE_LENGTH_ASM];
  for (int i = 0; i < *basic_num_lines; i++) {
    new_basic_num_lines = new_basic_num_lines + 1;
    new_basic_file[new_basic_num_lines] = malloc(MAX_NUM_LINES_BAS);
    strcpy(entire, basic_file[i]);
    if (EQS(strtok(entire, " "), "WHILE")) {
      cond_stack[diff] = malloc(MAX_COND_LENGTH);
      strcpy(cond_stack[diff], strtok(NULL, "\n"));
      sprintf(new_basic_file[new_basic_num_lines], "GOTO .L%d\n", start_val + diff * 2);

      new_basic_num_lines = new_basic_num_lines + 1;
      new_basic_file[new_basic_num_lines] = malloc(MAX_NUM_LINES_BAS);
      sprintf(new_basic_file[new_basic_num_lines], ".L%d:\n", start_val + diff * 2 + 1);

      diff++;
      
      if (start_val + diff * 2 > *num_generated_labels) {
        *num_generated_labels = start_val + diff * 2;
      }
    } else if (EQS(entire, "WEND\n")) {
      diff--;

      sprintf(new_basic_file[new_basic_num_lines], ".L%d:\n", start_val + diff * 2);

      new_basic_num_lines = new_basic_num_lines + 1;
      new_basic_file[new_basic_num_lines] = malloc(MAX_NUM_LINES_BAS);
      sprintf(new_basic_file[new_basic_num_lines], "IF %s THEN GOTO .L%d\n", cond_stack[diff], start_val + diff * 2 + 1);

      free(cond_stack[diff]);
    } else {
      strcpy(new_basic_file[new_basic_num_lines], basic_file[i]);
    }
  }
  free_file(basic_file, *basic_num_lines);
  free(cond_stack);
  *basic_num_lines = new_basic_num_lines + 1;
  return new_basic_file;
}

char **replace_for_with_while(char **basic_file, int *basic_num_lines) {
  int new_basic_num_lines = -1;
  char **var_stack = malloc(MAX_INNER_WHILE_LOOPS);
  char **new_basic_file = malloc(MAX_NUM_LINES_BAS * sizeof(char *));
  static const int MAX_FOR_LOOP_ASSIGNMENT_LENGTH = 25;

  int diff = 0;
  char entire[MAX_LINE_LENGTH_ASM];
  for (int i = 0; i < *basic_num_lines; i++) {
    new_basic_num_lines = new_basic_num_lines + 1;
    new_basic_file[new_basic_num_lines] = malloc(MAX_NUM_LINES_BAS);
    strcpy(entire, basic_file[i]);
    char first_word[MAX_LINE_LENGTH_BAS];
    strcpy(first_word, strtok(entire, " "));

    if (EQS(first_word, "FOR")) {
      var_stack[diff] = malloc(MAX_FOR_LOOP_ASSIGNMENT_LENGTH);
      strcpy(var_stack[diff], strtok(NULL, " "));
      sprintf(new_basic_file[new_basic_num_lines], "LET %s\n", var_stack[diff]);

      if (!EQS(strtok(NULL, " "), "TO")) {
        fprintf(stderr, "Compiler error: FOR..TO.. loop syntax error\n");
        exit(EXIT_FAILURE);
      }

      char *term_num = strtok(NULL, "\n");
      int term = 0;
      if (!EQS(term_num, "0")) {
        term = strtol(term_num, NULL, 10);
        if (term == 0) {
          fprintf(stderr, "Compiler error: FOR..TO.. loop termination syntax error\n");
          exit(EXIT_FAILURE);
        }
      }

      char assignment[MAX_FOR_LOOP_ASSIGNMENT_LENGTH];
      strcpy(assignment, var_stack[diff]);
      char variable[MAX_LINE_LENGTH_BAS];
      strcpy(variable, strtok(assignment, "="));

      new_basic_num_lines = new_basic_num_lines + 1; 
      new_basic_file[new_basic_num_lines] = malloc(MAX_NUM_LINES_BAS);
      sprintf(new_basic_file[new_basic_num_lines], "WHILE %s<=%s\n", variable, term_num);

      diff++;
    } else if (EQS(first_word, "NEXT")) {
      diff--;

      char var[MAX_NUM_LINES_BAS];
      strcpy(var, strtok(NULL, "\n"));
      if (!EQS(var, strtok(var_stack[diff], "="))) {
        fprintf(stderr, "Compiler error: variable in FOR..TO.. and NEXT inconsistent\n");
        exit(EXIT_FAILURE);
      }
      
      sprintf(new_basic_file[new_basic_num_lines], "LET %s=%s+1\n", var, var);

      new_basic_num_lines = new_basic_num_lines + 1;
      new_basic_file[new_basic_num_lines] = malloc(MAX_NUM_LINES_BAS);
      strcpy(new_basic_file[new_basic_num_lines], "WEND\n");

      free(var_stack[diff]);
    } else {
      strcpy(new_basic_file[new_basic_num_lines], basic_file[i]);
    }
  }
  free_file(basic_file, *basic_num_lines);
  free(var_stack);
  *basic_num_lines = new_basic_num_lines + 1;
  return new_basic_file;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  char basic_file_name[FILE_NAME_LENGTH];
  char assembly_file_name[FILE_NAME_LENGTH];
  if (argc < 3) {
    printf("Compiler requires two command line arguments\n");
    exit(EXIT_FAILURE);
  } else if (argc > 3) {
    printf("Compiler received more arguments than necessary\n");
    exit(EXIT_FAILURE);
  }
  strcpy(basic_file_name, argv[1]);
  strcpy(assembly_file_name, argv[2]);
  printf("%s\n", "BASIC");
  int *basic_num_lines = malloc(sizeof(int *));
  int *asm_num_lines = malloc(sizeof(int *));
  int *num_generated_labels = malloc(sizeof(int *));
  *num_generated_labels = 0;
  char **basic_file = read_file(basic_file_name, basic_num_lines);

  basic_file = replace_for_with_while(basic_file, basic_num_lines);
  replace_on_with_if(basic_file, *basic_num_lines);
  replace_do_loop_with_if(basic_file, basic_num_lines, num_generated_labels);
  basic_file = replace_while_wend_with_if(basic_file, basic_num_lines, num_generated_labels);
  
  printf("\n%s\n", "SIMPLIFIED BASIC");
  for (int i = 0; i < *basic_num_lines; i++) {
    printf("%s", basic_file[i]);
  }
  char *labels[MAX_NUM_LINES_ASM];
  int num_labels = get_labels(labels, basic_file, *basic_num_lines);
  char **assembly_file = compile_instructions(basic_file, *basic_num_lines, asm_num_lines, *num_generated_labels, labels, num_labels);
  printf("\n%s\n", "ASSEMBLY");
  write_file(assembly_file, assembly_file_name, *asm_num_lines);
  free_file(basic_file, *basic_num_lines);
  free_file(assembly_file, *asm_num_lines);
  free(basic_num_lines);
  free(asm_num_lines);
  free(num_generated_labels);
  return EXIT_SUCCESS;
}

