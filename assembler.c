#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "is.h"

#define LINE_SIZE 256
#define JUMP_TABLE_SIZE 64

typedef struct {
	char label[64];
	word position;
} jump_ent;

void load_registers_table();
uint8_t load_jump_table(FILE *input_file);
word label_to_position(char *label);
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

	char line_buffer[LINE_SIZE];
	char cleaned_buffer[LINE_SIZE];
	char op[8], rx[8], value[64];

	word memory_word;
	long converted_value = 0;

	while(fgets(line_buffer, LINE_SIZE, input_file) != NULL) {
		sscanf(line_buffer, "%[^\n]s%*[\n]]", cleaned_buffer);
		sscanf(cleaned_buffer, "%*[ \t]%[^]s]", cleaned_buffer);

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
			} else if(strcmp(op, "LDI") == 0) {
				memory_word = memory_word | (LDI << 11);
			} else if(strcmp(op, "ADI") == 0) {
				memory_word = memory_word | (ADI << 11);
			} else if(strcmp(op, "JMP") == 0) {
				memory_word = memory_word | (JMP << 11);
			} else if(strcmp(op, "LDR") == 0) {
				memory_word = memory_word | (LDR << 11);
			} else if(strcmp(op, "SVR") == 0) {
				memory_word = memory_word | (SVR << 11);
			} else if(strcmp(op, "SBI") == 0) {
				memory_word = memory_word | (SBI << 11);
			} else if(strcmp(op, "SLI") == 0) {
				memory_word = memory_word | (SLI << 11);
			} else if(strcmp(op, "SRI") == 0) {
				memory_word = memory_word | (SRI << 11);
			} else if(strcmp(op, "CAL") == 0) {
				memory_word = memory_word | (CAL << 11);
			} else if(strcmp(op, "RET") == 0) {
				memory_word = memory_word | (RET << 11);
			} else if(strcmp(op, "ADD") == 0) {
				memory_word = memory_word | (ADD << 11);
			} else if(strcmp(op, "SUB") == 0) {
				memory_word = memory_word | (SUB << 11);
			} else if(strcmp(op, "PSH") == 0) {
				memory_word = memory_word | (PSH << 11);
			} else if(strcmp(op, "POP") == 0) {
				memory_word = memory_word | (POP << 11);
			} else if(strcmp(op, "CMP") == 0) {
				memory_word = memory_word | (CMP << 11);
			} else if(strcmp(op, "BRE") == 0) {
				memory_word = memory_word | (BRE << 11);
			} else if(strcmp(op, "BRN") == 0) {
				memory_word = memory_word | (BRN << 11);
			} else if(strcmp(op, "CPY") == 0) {
				memory_word = memory_word | (CPY << 11);
			} else {
				printf("Unknown operation: %s\n", op);
				printf("Defaulting to NOP\n");
				memory_word = memory_word | (NOP << 11);
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
			}

			errno = 0;
			converted_value = strtol(value, NULL, 0);

			if(errno == 0 && converted_value != 0) {
				memory_word = memory_word | converted_value;
			} else {
				memory_word = memory_word | label_to_position(value);
			}

			fwrite(&memory_word, sizeof(word), 1, output_file);
		}
	}

	print_jump_table();

	fclose(input_file);
	fclose(output_file);

	return 0;
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

	jump_ent_count = 8;
	first_unknown_jump_ent = 8;
}

uint8_t load_jump_table(FILE *input_file) {
	char line_buffer[LINE_SIZE];
	char cleaned_buffer[LINE_SIZE];
	char label[64];

	word current_address = 0;

	while(fgets(line_buffer, LINE_SIZE, input_file) != NULL) {
		sscanf(line_buffer, "%[^\n]s%*[\n]]", cleaned_buffer);
		sscanf(cleaned_buffer, "%*[ \t]%[^]s]", cleaned_buffer);

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

	rewind(input_file);

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

void print_jump_table() {
	printf("=== Jump map (%d jump pairs) ===\n", jump_ent_count);

	for(word i = 0; i < jump_ent_count; i++) {
		printf("%s -> %04X\n", jump_table[i].label, jump_table[i].position);
	}

	printf("====================\n");
}
