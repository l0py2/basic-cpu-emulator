#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "is.h"

#define STR(x) "" #x ""

#define LINE_SIZE 1024
#define JUMP_TABLE_SIZE 256

typedef struct {
	char label[64];
	word position;
} jump_ent;

void clean_instruction_line(char *line_buffer, char *cleaned_buffer);
void load_registers_table();
uint8_t load_jump_table(FILE *input_file);
word label_to_position(char *label);
uint8_t write_instructions(FILE *input_file, FILE *output_file);
void print_jump_table();

static jump_ent jump_table[JUMP_TABLE_SIZE];
static word first_unknown_jump_ent = 0;
static word jump_ent_count = 0;

int main(int argc, char **argv) {
	if(argc < 3) {
		printf("Usage: %s [input path] [output path]\n", argv[0]);
		return 1;
	}

	FILE *input_file = fopen(argv[1], "r");

	if(input_file == NULL) {
		printf("Error opening input file\n");
		return 1;
	}

	FILE *output_file = fopen(argv[2], "wb");

	if(output_file == NULL) {
		fclose(input_file);
		printf("Error opening output file\n");
		return 1;
	}

	load_registers_table();

	if(!load_jump_table(input_file)) {
		fclose(input_file);
		fclose(output_file);
		printf("Error loading jump table\n");
		return 1;
	}

	rewind(input_file);

	if(!write_instructions(input_file, output_file)) {
		fclose(input_file);
		fclose(output_file);
		printf("Error writting instructions\n");
		return 1;
	}

	print_jump_table();

	fclose(input_file);
	fclose(output_file);

	return 0;
}

void clean_instruction_line(char *line_buffer, char *cleaned_buffer) {
	char *line_buffer_copy = line_buffer;
	char *cleaned_buffer_copy = cleaned_buffer;

	*cleaned_buffer_copy = '\0';

	while(*line_buffer_copy == ' ' || *line_buffer_copy == '\t') {
		line_buffer_copy++;
	}

	while(*line_buffer_copy != '\n') {
		*cleaned_buffer_copy = *line_buffer_copy;

		cleaned_buffer_copy++;
		line_buffer_copy++;
	}

	*cleaned_buffer_copy = '\0';
}

void load_registers_table() {
	strcpy(jump_table[0].label, "R1");
	jump_table[0].position = R1;
	strcpy(jump_table[1].label, "R2");
	jump_table[1].position = R2;
	strcpy(jump_table[2].label, "R3");
	jump_table[2].position = R3;
	strcpy(jump_table[3].label, "R4");
	jump_table[3].position = R4;
	strcpy(jump_table[4].label, "R5");
	jump_table[4].position = R5;
	strcpy(jump_table[5].label, "R6");
	jump_table[5].position = R6;
	strcpy(jump_table[6].label, "R7");
	jump_table[6].position = R7;
	strcpy(jump_table[7].label, "R8");
	jump_table[7].position = R8;
	strcpy(jump_table[8].label, "FLAGS");
	jump_table[8].position = FLAGS;

	jump_ent_count = 9;
	first_unknown_jump_ent = 9;
}

uint8_t load_jump_table(FILE *input_file) {
	char line_buffer[LINE_SIZE];
	char cleaned_buffer[LINE_SIZE];
	char label[64];

	word current_address = 0;

	while(fgets(line_buffer, LINE_SIZE, input_file) != NULL) {
		clean_instruction_line(line_buffer, cleaned_buffer);

		// Empty line
		if(cleaned_buffer[0] == '\0') {
			continue;
		}

		// Comment
		if(cleaned_buffer[0] == '#') {
			continue;
		}


		if(sscanf(cleaned_buffer, "%*s %*s %*s") != EOF) {
			for(word i = first_unknown_jump_ent; i < jump_ent_count; i++) {
				jump_table[i].position = current_address;
			}

			first_unknown_jump_ent = jump_ent_count;

			current_address++;
		} else if(sscanf(cleaned_buffer, "%64[0-9a-zA-Z_]s:", &label) == 1) {
			strcpy(jump_table[jump_ent_count].label, label);
			jump_table[jump_ent_count].position = 0;

			jump_ent_count++;

			if(jump_ent_count >= JUMP_TABLE_SIZE) {
				return 0;
			}
		}
	}

	return 1;
}

word label_to_position(char *label) {
	for(word i = 0; i < jump_ent_count; i++) {
		if(strcmp(jump_table[i].label, label) == 0) {
			return jump_table[i].position;
		}
	}

	return 0;
}

uint8_t write_instructions(FILE *input_file, FILE *output_file) {
	char line_buffer[LINE_SIZE];
	char cleaned_buffer[LINE_SIZE];
	char op[8], rx[8], value[64];

	word memory_word;
	unsigned long converted_value = 0;

	while(fgets(line_buffer, LINE_SIZE, input_file) != NULL) {
		clean_instruction_line(line_buffer, cleaned_buffer);

		// Empty line
		if(cleaned_buffer[0] == '\0') {
			continue;
		}

		// Comment
		if(cleaned_buffer[0] == '#') {
			continue;
		}

		// Label
		if(sscanf(cleaned_buffer, "%*[^:]%*[:]") == 0) {
			continue;
		}

		if(sscanf(cleaned_buffer, "%8s %4s %64s", op, rx, value) == 3) {
			memory_word = 0;

			if(strcmp(op, "NOP") == 0) {
				memory_word = memory_word | (NOP << 11);
			} else if(strcmp(op, "LDHI") == 0) {
				memory_word = memory_word | (LDHI << 11);
			} else if(strcmp(op, "LDLI") == 0) {
				memory_word = memory_word | (LDLI << 11);
			} else if(strcmp(op, "LDM") == 0) {
				memory_word = memory_word | (LDM << 11);
			} else if(strcmp(op, "SVM") == 0) {
				memory_word = memory_word | (SVM << 11);
			} else if(strcmp(op, "COPY") == 0) {
				memory_word = memory_word | (COPY << 11);
			} else if(strcmp(op, "ADDI") == 0) {
				memory_word = memory_word | (ADDI << 11);
			} else if(strcmp(op, "SUBI") == 0) {
				memory_word = memory_word | (SUBI << 11);
			} else if(strcmp(op, "COMPI") == 0) {
				memory_word = memory_word | (COMPI << 11);
			} else if(strcmp(op, "SLI") == 0) {
				memory_word = memory_word | (SLI << 11);
			} else if(strcmp(op, "SRI") == 0) {
				memory_word = memory_word | (SRI << 11);
			} else if(strcmp(op, "ADD") == 0) {
				memory_word = memory_word | (ADD << 11);
			} else if(strcmp(op, "SUB") == 0) {
				memory_word = memory_word | (SUB << 11);
			} else if(strcmp(op, "COMP") == 0) {
				memory_word = memory_word | (COMP << 11);
			} else if(strcmp(op, "JUMP") == 0) {
				memory_word = memory_word | (JUMP << 11);
			} else if(strcmp(op, "BRNEQ") == 0) {
				memory_word = memory_word | (BRNEQ << 11);
			} else if(strcmp(op, "BREQ") == 0) {
				memory_word = memory_word | (BREQ << 11);
			} else if(strcmp(op, "CALL") == 0) {
				memory_word = memory_word | (CALL << 11);
			} else if(strcmp(op, "PUSH") == 0) {
				memory_word = memory_word | (PUSH << 11);
			} else if(strcmp(op, "POP") == 0) {
				memory_word = memory_word | (POP << 11);
			} else if(strcmp(op, "RET") == 0) {
				memory_word = memory_word | (RET << 11);
			} else {
				printf("Unknown operation: %s\n", op);
				return 0;
			}

			if(strcmp(rx, "R1") == 0) {
				memory_word = memory_word | (R1 << 8);
			} else if(strcmp(rx, "R2") == 0) {
				memory_word = memory_word | (R2 << 8);
			} else if(strcmp(rx, "R3") == 0) {
				memory_word = memory_word | (R3 << 8);
			} else if(strcmp(rx, "R4") == 0) {
				memory_word = memory_word | (R4 << 8);
			} else if(strcmp(rx, "R5") == 0) {
				memory_word = memory_word | (R5 << 8);
			} else if(strcmp(rx, "R6") == 0) {
				memory_word = memory_word | (R6 << 8);
			} else if(strcmp(rx, "R7") == 0) {
				memory_word = memory_word | (R7 << 8);
			} else if(strcmp(rx, "R8") == 0) {
				memory_word = memory_word | (R8 << 8);
			} else if(strcmp(rx, "FLAGS") == 0) {
				memory_word = memory_word | (FLAGS << 8);
			}

			errno = 0;
			converted_value = strtoul(value, NULL, 0);

			if(errno == 0 && converted_value != 0) {
				memory_word = memory_word | converted_value;
			} else {
				memory_word = memory_word | label_to_position(value);
			}

			fwrite(&memory_word, sizeof(word), 1, output_file);
		} else {
			printf("Invalid line: %s\n", cleaned_buffer);
			return 0;
		}
	}

	return 1;
}

void print_jump_table() {
	printf("=== Jump map (%d jump pairs) ===\n", jump_ent_count);

	for(word i = 0; i < jump_ent_count; i++) {
		printf("%s -> %04X\n", jump_table[i].label, jump_table[i].position);
	}

	printf("====================\n");
}
