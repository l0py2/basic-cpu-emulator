#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE 256
#define REGISTER_COUNT 8

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
	SRI = 0b1000,
	CAL = 0b1001,
	RET = 0b1010,
	ADD = 0b1011,
	SUB = 0b1100,
	PSH = 0b1101,
	POP = 0b1110
} instruction;

typedef enum {
	R1 = 0b000,
	R2 = 0b001,
	R3 = 0b010,
	R4 = 0b011,
	R5 = 0b100,
	R6 = 0b101,
	R7 = 0b110,
	R8 = 0b111
} rx;

word extract_register(word instruction);
word extract_value(word instruction);
void print_memory(word *memory, word memory_size);

int main(void) {
	word pc = 0;
	word sp = MEMORY_SIZE - 1;
	word memory[MEMORY_SIZE];
	word registers[REGISTER_COUNT];

	for(word i = 0; i < MEMORY_SIZE; i++) {
		memory[i] = NOP;
	}

	for(word i = 0; i < REGISTER_COUNT; i++) {
		registers[i] = 0;
	}

	// [opcode 4 bits] [register 3 bits] [value 8 bits]
	memory[0] = (NOP << 12);

	// 1 - 3
	memory[1] = (LDI << 12) | (R3 << 9) | 1;
	memory[2] = (SBI << 12) | (R3 << 9) | 3;

	// R6 -> stack -> R7
	memory[3] = (LDI << 12) | (R6 << 9) | 1;
	memory[4] = (PSH << 12) | (R6 << 9);
	memory[5] = (NOP << 12);
	memory[6] = (POP << 12) | (R7 << 9);

	// 1000 - 1
	memory[7] = (LDI << 12) | (R1 << 9) | ((1000 & 0xff00) >> 8); // First byte
	memory[8] = (SLI << 12) | (R1 << 9) | 8;
	memory[9] = (ADI << 12) | (R1 << 9) | (1000 & 0x00ff); // Second byte
	memory[10] = (SBI << 12) | (R1 << 9) | 1;

	// R1 -> 0x1f -> R2
	memory[11] = (SVR << 12) | (R1 << 9) | 0x1f;
	memory[12] = (LDR << 12) | (R2 << 9) | 0x1f;

	// Call R4 = R4 + R5 to solve 2 + 3
	memory[13] = (LDI << 12) | (R4 << 9) | 2;
	memory[14] = (LDI << 12) | (R5 << 9) | 3;
	memory[15] = (CAL << 12) | 0x3a;

	// Infinite R1++
	memory[16] = (ADI << 12) | (R1 << 9) | 1;
	memory[17] = (JMP << 12) | 16;

 	// Ignored instruction
	memory[18] = (LDI << 12) | (R2 << 9) | 15;

 	// Random instructions for padding
	memory[0x38] = 0xffff;
	memory[0x39] = 0xffff;
	// R4 = R4 + R5
	memory[0x3a] = (ADD << 12) | (R4 << 9) | (R5 << 1);
	memory[0x3b] = (RET << 12);

	for(;;) {
		printf("PC: %04X\n", pc);
		printf("SP: %04X\n", sp);
		printf("Current instruction: %04X\n", memory[pc]);
		printf("=== Memory ===\n");
		print_memory(memory, MEMORY_SIZE);
		printf("\n=== Registers ===\n");
		print_memory(registers, REGISTER_COUNT);
		printf("\n\nPress return to cycle\n");
		getchar();
		printf("=============================================\n\n");

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
				pc = extract_value(memory[pc]);
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
			case CAL:
				memory[sp] = pc + 1;
				sp--;
				pc = extract_value(memory[pc]);
				break;
			case RET:
				sp++;
				pc = memory[sp];
				break;
			case ADD:
				registers[extract_register(memory[pc])] += registers[extract_register(extract_value(memory[pc]) << 8)];
				pc++;
				break;
			case SUB:
				registers[extract_register(memory[pc])] -= registers[extract_register(extract_value(memory[pc]) << 8)];
				pc++;
				break;
			case PSH:
				memory[sp] = registers[extract_register(memory[pc])];
				sp--;
				pc++;
				break;
			case POP:
				sp++;
				registers[extract_register(memory[pc])] = memory[sp];
				pc++;
				break;
			default:
				pc++;
				break;
		}
	}

	return 0;
}

word extract_register(word instruction) {
	return (instruction & 0b0000111000000000) >> 9;
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
