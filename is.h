#ifndef IS_H
#define IS_H

#include <stdint.h>

#define MEMORY_SIZE 65536 // 2^16 addresses
#define REGISTER_COUNT 8

typedef uint16_t word;

typedef enum {
	NOP = 0b00000,
	LDHI,
	LDLI,
	LDM,
	SVM,
	COPY,
	ADDI,
	SUBI,
	COMPI,
	SLI,
	SRI,
	CLR,
	SET,
	NOT,
	ADD,
	SUB,
	COMP,
	AND,
	OR,
	JUMP,
	BRNEQ,
	BREQ,
	CALL,
	PUSH,
	POP,
	RET
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
	FLAGS = R8
} rx;

typedef enum {
	ZERO     = 0b1000000000000000,
	OVERFLOW = 0b0100000000000000
} flags;

#endif
