#include "example-program.h"

#include "is.h"

static word auto_address = 0;

void write_instruction(word *memory, word address, word op, word reg, word value);

void write_example(word *memory) {
	write_instruction(memory, 0, LDI, R1, 2);
	write_instruction(memory, 0, LDI, R2, 3);
	write_instruction(memory, 0, CAL, 0, 0x3a);
	write_instruction(memory, 0, JMP, 0, 0);

	write_instruction(memory, 0x3a, ADD, R1, R2);
	write_instruction(memory, 0x3b, RET, 0, 0);
}

void write_instruction(word *memory, word address, word op, word reg, word value) {
	// [opcode 4 bits] [register 4 bits] [value 8 bits]
	if(address == 0) {
		memory[auto_address] = (op << 12) | (reg << 8) | value;
		auto_address++;
	} else {
		memory[address] = (op << 12) | (reg << 8) | value;
	}
}
