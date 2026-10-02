#include "example-program.h"

#include "is.h"

static word auto_address = 0;

void write_instruction(word *memory, word address, word op, word reg, word value);

void write_example(word *memory) {
	write_instruction(memory, 0, NOP, 0, 0);

	// 1 - 3
	write_instruction(memory, 0, LDI, R3, 1);
	write_instruction(memory, 0, SBI, R3, 3);

	// R6 -> stack -> R7
	write_instruction(memory, 0, LDI, R6, 1);
	write_instruction(memory, 0, PSH, R6, 0);
	write_instruction(memory, 0, NOP, 0, 0);
	write_instruction(memory, 0, POP, R7, 0);

	// 1000 - 1
	write_instruction(memory, 0, LDI, R1, (1000 & 0xff00) >> 8); // First byte
	write_instruction(memory, 0, SLI, R1, 8);
	write_instruction(memory, 0, ADI, R1, 1000 & 0x00ff);        // Second byte
	write_instruction(memory, 0, SBI, R1, 1);

	// R1 -> 0x1f -> R2
	write_instruction(memory, 0, SVR, R1, 0x1f);
	write_instruction(memory, 0, LDR, R2, 0x1f);

	// Call R4 = R4 + R5 to solve 2 + 3
	write_instruction(memory, 0, LDI, R4, 2);
	write_instruction(memory, 0, LDI, R5, 3);
	write_instruction(memory, 0, CAL, 0, 0x3a);

	// Infinite R1++
	write_instruction(memory, 0, ADI, R1, 1);
	write_instruction(memory, 0, JMP, 0, auto_address - 1);

 	// Ignored instruction
	write_instruction(memory, 0, LDI, R2, 15);

 	// Random instructions for padding
	write_instruction(memory, 0x38, 0xf, 0b111, 0xff);
	write_instruction(memory, 0x39, 0xf, 0b111, 0xff);
	// R4 = R4 + R5
	write_instruction(memory, 0x3a, ADD, R4, R5 << 1);
	write_instruction(memory, 0x3b, RET, 0, 0);
}

void write_instruction(word *memory, word address, word op, word reg, word value) {
	// [opcode 4 bits] [register 3 bits] [value 8 bits]
	if(address == 0) {
		memory[auto_address] = (op << 12) | (reg << 9) | value;
		auto_address++;
	} else {
		memory[address] = (op << 12) | (reg << 9) | value;
	}
}
