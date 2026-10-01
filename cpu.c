#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 256
#define REGISTER_COUNT 4

typedef uint16_t word;

typedef enum {
	NOP = 0b0000,
	LDI = 0b0001,
	ADI = 0b0010,
	JMP = 0b0011,
	LDR = 0b0100,
	SVR = 0b0101,
	SBI = 0b0110,
	SLI = 0b0111,
	SRI = 0b1000
} instruction;

typedef enum {
	R1 = 0b00,
	R2 = 0b01,
	R3 = 0b10,
	R4 = 0b11
} rx;

word extract_register(word instruction);
word extract_value(word instruction);
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
	memory[1] = (LDI << 12) | (R3 << 10) | 1;
	memory[2] = (SBI << 12) | (R3 << 10) | 3;
	memory[3] = (LDI << 12) | (R1 << 10) | ((1000 & 0xff00) >> 8); // First byte
	memory[4] = (SLI << 12) | (R1 << 10) | 8;
	memory[5] = (ADI << 12) | (R1 << 10) | (1000 & 0x00ff); // Second byte
	memory[6] = (SBI << 12) | (R1 << 10) | 1;
	memory[7] = (SVR << 12) | (R1 << 10) | 0xff;
	memory[8] = (LDR << 12) | (R2 << 10) | 0xff;
	memory[9] = (ADI << 12) | (R1 << 10) | 1;
	memory[10] = (JMP << 12) | 9;
	memory[11] = (LDI << 12) | (R2 << 10) | 15;

	for(;;) {
		switch(memory[pc] >> 12) {
			case NOP:
				pc++;
				break;
			case LDI:
				registers[extract_register(memory[pc])] = extract_value(memory[pc]);
				pc++;
				break;
			case ADI:
				registers[extract_register(memory[pc])] += extract_value(memory[pc]);
				pc++;
				break;
			case JMP:
				pc = memory[pc] & 0x00ff;
				break;
			case LDR:
				registers[extract_register(memory[pc])] = memory[extract_value(memory[pc])];
				pc++;
				break;
			case SVR:
				memory[extract_value(memory[pc])] = registers[extract_register(memory[pc])];
				pc++;
				break;
			case SBI:
				registers[extract_register(memory[pc])] -= extract_value(memory[pc]);
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

word extract_register(word instruction) {
	return (instruction & 0b0000110000000000) >> 10;
}

word extract_value(word instruction) {
	return instruction & 0x00ff;
}

void print_memory(word *memory, word memory_size) {
	uint8_t line_break = 1;

	for(word i; i < memory_size; i++) {
		printf("%04X ", memory[i]);

		if(line_break > 15) {
			line_break = 1;
			printf("\n");
		} else {
			line_break++;
		}
	}
}
