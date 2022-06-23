#define _GNU_SOURCE
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define NUM_TEST_CASES 25
#define TEST_STRING_1 "./compiler test_cases/"
#define TEST_STRING_2 ".bas test_temp/"
#define DOT_S ".s"
#define TEST_CASES_DIR "test_cases/"
#define TEST_TEMP_DIR "test_temp/"
#define MAXIMUM_LEN 256

static const char TEST_CASES[NUM_TEST_CASES][MAXIMUM_LEN] = {
	"add01", "add02", "for", "rem",
	"while01", "while02", "sub01",
	"sub02", "doloop", "len_literal",
	"long_division", "remainder", "rnd",
	"string_print", "gosub", "len",
	"guess", "input", "input_string",
	"input_string2", "on_gosub", "mult",
	"stringeq", "doubleif", "rock_paper_scissors"
	
};

int main() {

	int tests_passed = 0;

	for (int i = 0; i < NUM_TEST_CASES; i++) {

		// run compiler on each test case
		char sys_string[MAXIMUM_LEN];
		strcpy(sys_string, TEST_STRING_1);
		strcat(sys_string, TEST_CASES[i]);
		strcat(sys_string, TEST_STRING_2);
        strcat(sys_string, TEST_CASES[i]);
		strcat(sys_string, DOT_S);

		// check that compiler terminated without errors
		assert(!system(sys_string));

		// compare .s made by compiler with model .s
		char model_location[MAXIMUM_LEN];
		strcpy(model_location, TEST_CASES_DIR);
		strcat(model_location, TEST_CASES[i]);
		strcat(model_location, DOT_S);
		FILE *model = fopen(model_location, "r");

		char compiled_location[MAXIMUM_LEN];
		strcpy(compiled_location, TEST_TEMP_DIR);
		strcat(compiled_location, TEST_CASES[i]);
		strcat(compiled_location, DOT_S);
		FILE *compiled = fopen(compiled_location, "r");

		char *model_line = NULL;
		char *compiled_line = NULL;

		size_t *model_len = 0;
		size_t *compiled_len = 0;

		int num_errors = 0;
		int line = 1;

		while (getline(&model_line, model_len, model) != -1 &&
		  getline(&compiled_line, compiled_len, compiled) != -1) {
			if (strcmp(model_line, compiled_line)) {
				num_errors++;
				printf("Error in %s on line %d\n", TEST_CASES[i], line);
				printf("Compiler produces\n %s but model answer has\n %s\n", compiled_line, model_line);
			}
			line++;
		}

		if (num_errors == 0) {
			printf("No errors in %s.\n", TEST_CASES[i]);
			tests_passed++;
		}

	}

	printf("TESTS PASSED: %d / %d\n", tests_passed, NUM_TEST_CASES);

}
