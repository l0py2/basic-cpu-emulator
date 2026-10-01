#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 0xff
#define REGISTER_COUNT 4

typedef uint16_t word;

typedef enum {
	NOP = 0b00,
	LDI = 0b01,
	ADI = 0b10,
	JMP = 0b11
} instruction;

typedef enum {
	R1 = 0b00,
	R2 = 0b01,
	R3 = 0b10,
	R4 = 0b11
} rx;

void print_memory(word *memory, word memory_size);

int main(void) {
	word pc = 0;
	word memory[MEMORY_SIZE];
	word registers[REGISTER_COUNT];

	for(word i = 0; i < MEMORY_SIZE; i++) {
		memory[i] = NOP;
	}

	for(word i = 0; i < REGISTER_COUNT; i++) {
		registers[i] = 0;
	}

	// [opcode 2 bits] [register 2 bits] [value 8-12 bits]
	memory[0] = (NOP << 14);
	memory[1] = (LDI << 14) | (R1 << 12) | 1;
	memory[2] = (ADI << 14) | (R1 << 12) | 1;
	memory[3] = (JMP << 14) | 2;

	for(;;) {
		switch(memory[pc] >> 14) {
			case NOP:
				pc++;
				break;
			case LDI:
				registers[memory[pc] & 0x3000] = memory[pc] & 0x0fff;
				pc++;
				break;
			case ADI:
				registers[memory[pc] & 0x3000] += memory[pc] & 0x0fff;
				pc++;
				break;
			case JMP:
				pc = memory[pc] & 0x00ff;
				break;
			default:
				pc++;
				break;
		}

		printf("PC: %04X\n", pc);
		printf("=== Memory ===\n");
		print_memory(memory, MEMORY_SIZE);
		printf("\n=== Registers ===\n");
		print_memory(registers, REGISTER_COUNT);
		printf("\n\nPress return to cycle\n");
		getchar();
		printf("=============================================\n\n");
	}

	return 0;
}

void print_memory(word *memory, word memory_size) {
	word line_break = 0;

	for(word i; i < memory_size; i++) {
		printf("%04X ", memory[i]);

		if(line_break > 15) {
			line_break = 0;
			printf("\n");
		} else {
			line_break++;
		}
	}
}
