#ifndef IS_H
#define IS_H

#include <stdint.h>

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

#endif
