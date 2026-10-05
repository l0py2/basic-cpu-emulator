#include <stdint.h>
#include <stdio.h>

#include "is.h"
#include "tui.h"

#define PRINT_MEMORY_SIZE 256

void read_file(word *memory, unsigned int memory_size, char *path);
word extract_register(word instruction);
word extract_value(word instruction);
void execute();

static word pc = 0;
static word sp = MEMORY_SIZE - 1;
static word memory[MEMORY_SIZE];
static word registers[REGISTER_COUNT];

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Usage: %s [binary path]\n", argv[0]);
		return 1;
	}

	if(!initialize_tui()) {
		printf("Failed to initialize TUI\n");
		return 1;
	}

	for(unsigned int i = 0; i < MEMORY_SIZE; i++) {
		memory[i] = NOP;
	}

	for(word i = 0; i < REGISTER_COUNT; i++) {
		registers[i] = 0;
	}

	read_file(memory, MEMORY_SIZE, argv[1]);

	int key;
	unsigned int line = 0;

	for(;;) {
		print_help_view();
		print_registers_view(pc, sp, registers, REGISTER_COUNT);
		print_memory_view(memory, MEMORY_SIZE, line);

		refresh_tui();

		key = getch();

		if(key == 'q') {
			break;
		}

		switch(key) {
			case 'c':
				execute();
				break;
			case 'j':
				line++;
				break;
			case 'k':
				line--;
				break;
			case 's':
				line = 0;
				break;
			case 'e':
				line = (MEMORY_SIZE / 16) - 1;
				break;
		}

		if(line > ((MEMORY_SIZE / 16) - 1)) {
			line = 0;
		}
	}

	end_tui();

	return 0;
}

void read_file(word *memory, unsigned int memory_size, char *path) {
	FILE *file = fopen(path, "rb");

	if(file == NULL) {
		return;
	}

	word memory_word = 0;
	word current_address = 0;

	while(fread(&memory_word, 1, sizeof(word), file) && current_address < memory_size) {
		memory[current_address] = memory_word;
		current_address++;
	}

	fclose(file);
}

word extract_register(word instruction) {
	return (instruction & 0x0700) >> 8;
}

word extract_value(word instruction) {
	return instruction & 0x00ff;
}

void execute() {
	word temp = 0;

	switch(memory[pc] >> 11) {
		case NOP:
			pc++;
			break;
		case LDHI:
			registers[extract_register(memory[pc])] =
				(extract_value(memory[pc]) << 8)
				| (registers[extract_register(memory[pc])] & 0x00ff);
			pc++;
			break;
		case LDLI:
			registers[extract_register(memory[pc])] =
				extract_value(memory[pc])
				| (registers[extract_register(memory[pc])] & 0xff00);
			pc++;
			break;
		case LDM:
			registers[extract_register(memory[pc])] = memory[registers[extract_register(extract_value(memory[pc]) << 8)]];
			pc++;
			break;
		case SVM:
			memory[registers[extract_register(extract_value(memory[pc]) << 8)]] = registers[extract_register(memory[pc])];
			pc++;
			break;
		case COPY:
			registers[extract_register(memory[pc])] = registers[extract_register(extract_value(memory[pc]) << 8)];
			pc++;
			break;
		case ADDI:
			registers[extract_register(memory[pc])] += extract_value(memory[pc]);
			pc++;
			break;
		case SUBI:
			registers[extract_register(memory[pc])] -= extract_value(memory[pc]);
			pc++;
			break;
		case COMPI:
			temp = registers[extract_register(memory[pc])] - extract_value(memory[pc]);

			registers[FLAGS] = registers[FLAGS] & ~(ZERO | OVERFLOW);

			if(temp == 0) {
				registers[FLAGS] = registers[FLAGS] | ZERO;
			}

			if(temp > registers[extract_register(memory[pc])]) {
				registers[FLAGS] = registers[FLAGS] | OVERFLOW;
			}

			pc++;
			break;
		case SLI:
			registers[extract_register(memory[pc])] = registers[extract_register(memory[pc])] << extract_value(memory[pc]);
			pc++;
			break;
		case SRI:
			registers[extract_register(memory[pc])] = registers[extract_register(memory[pc])] >> extract_value(memory[pc]);
			pc++;
			break;
		case CLR:
			registers[extract_register(memory[pc])] = 0x0000;
			pc++;
			break;
		case SET:
			registers[extract_register(memory[pc])] = 0xffff;
			pc++;
			break;
		case NOT:
			registers[extract_register(memory[pc])] = ~registers[extract_register(memory[pc])];
			pc++;
			break;
		case ADD:
			registers[extract_register(memory[pc])] += registers[extract_register(extract_value(memory[pc]) << 8)];
			pc++;
			break;
		case SUB:
			registers[extract_register(memory[pc])] -= registers[extract_register(extract_value(memory[pc]) << 8)];
			pc++;
			break;
		case COMP:
			temp = registers[extract_register(memory[pc])]
				- registers[extract_register(extract_value(memory[pc]) << 8)];

			registers[FLAGS] = registers[FLAGS] & ~(ZERO | OVERFLOW);

			if(temp == 0) {
				registers[FLAGS] = registers[FLAGS] | ZERO;
			}

			if(temp > registers[extract_register(memory[pc])]) {
				registers[FLAGS] = registers[FLAGS] | OVERFLOW;
			}

			pc++;
			break;
		case AND:
			registers[extract_register(memory[pc])] &= registers[extract_register(extract_value(memory[pc]) << 8)];
			pc++;
			break;
		case OR:
			registers[extract_register(memory[pc])] |= registers[extract_register(extract_value(memory[pc]) << 8)];
			pc++;
			break;
		case JUMP:
			pc = extract_value(memory[pc]);
			break;
		case BRNEQ:
			if(registers[FLAGS] & ZERO) {
				pc++;
			} else {
				pc = extract_value(memory[pc]);
			}
			break;
		case BREQ:
			if(registers[FLAGS] & ZERO) {
				pc = extract_value(memory[pc]);
			} else {
				pc++;
			}
			break;
		case CALL:
			memory[sp] = pc + 1;
			sp--;
			pc = extract_value(memory[pc]);
			break;
		case PUSH:
			memory[sp] = registers[extract_register(memory[pc])];
			sp--;
			pc++;
			break;
		case POP:
			sp++;
			registers[extract_register(memory[pc])] = memory[sp];
			pc++;
			break;
		case RET:
			sp++;
			pc = memory[sp];
			break;
		default:
			pc++;
			break;
	}
}
