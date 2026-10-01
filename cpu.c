#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 0xff
#define REGISTER_COUNT 4

typedef uint16_t word;

typedef enum {
	NOP = 0b0000,
	LDI = 0b0001,
	ADI = 0b0010,
	JMP = 0b0011,
	LDR = 0b0100,
	SVR = 0b0101
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

	// [opcode 4 bits] [register 2 bits] [value 8 bits]
	memory[0] = (NOP << 12);
	memory[1] = (LDI << 12) | (R1 << 10) | 1;
	memory[2] = (ADI << 12) | (R1 << 10) | 1;
	memory[3] = (JMP << 12) | 2;

	for(;;) {
		switch(memory[pc] >> 12) {
			case NOP:
				pc++;
				break;
			case LDI:
				registers[memory[pc] & 0xc000] = memory[pc] & 0x00ff;
				pc++;
				break;
			case ADI:
				registers[memory[pc] & 0xc000] += memory[pc] & 0x00ff;
				pc++;
				break;
			case JMP:
				pc = memory[pc] & 0x00ff;
				break;
			case LDR:
				registers[memory[pc] & 0xc000] = memory[memory[pc] & 0x00ff];
				pc++;
				break;
			case SVR:
				memory[memory[pc] & 0x00ff] = registers[memory[pc] & 0xc000];
				pc++;
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
