#ifndef IS_H
#define IS_H

#include <stdint.h>

#define MEMORY_SIZE 256
#define REGISTER_COUNT 8

typedef uint16_t word;

typedef enum {
	NOP = 0b00000,
	LDI = 0b00001,
	ADI = 0b00010,
	JMP = 0b00011,
	LDR = 0b00100,
	SVR = 0b00101,
	SBI = 0b00110,
	SLI = 0b00111,
	SRI = 0b01000,
	CAL = 0b01001,
	RET = 0b01010,
	ADD = 0b01011,
	SUB = 0b01100,
	PSH = 0b01101,
	POP = 0b01110,
	CMP = 0b01111,
	BRE = 0b10000,
	BRN = 0b10001,
	CPY = 0b10010
} instruction;

typedef enum {
	R1 = 0b000,
	R2 = 0b001,
	R3 = 0b010,
	R4 = 0b011,
	R5 = 0b100,
	R6 = 0b101,
	R7 = 0b110,
	R8 = 0b111,
	FG = 0b111
} rx;

typedef enum {
	ZERO     = 0b1000000000000000,
	OVERFLOW = 0b0100000000000000
} flags;

#endif
