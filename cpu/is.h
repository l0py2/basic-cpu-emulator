#ifndef IS_H
#define IS_H

#include <stdint.h>
#include <unistd.h>

#define MEMORY_SIZE 65536 // 2^16 addresses
#define REGISTER_COUNT 8

typedef uint16_t word;

typedef enum {
	NOP  = 0b00000 << 11,
	LDHI = 0b00001 << 11,
	LDLI = 0b00010 << 11,
	ADD  = 0b00011 << 11,
	SUB  = 0b00100 << 11,
	NEG  = 0b00101 << 11,
	MULT = 0b00110 << 11,
	DIV  = 0b00111 << 11,
	REM  = 0b01000 << 11,
	NOT  = 0b01001 << 11,
	AND  = 0b01010 << 11,
	OR   = 0b01011 << 11,
	XOR  = 0b01100 << 11,
	SL   = 0b01101 << 11,
	SR   = 0b01110 << 11,
	JUMP = 0b01111 << 11,
	BREQ = 0b10000 << 11,
	BRNE = 0b10001 << 11,
	BRGT = 0b10010 << 11,
	BRLT = 0b10011 << 11,
	LDM  = 0b10100 << 11,
	SVM  = 0b10101 << 11,
	PUSH = 0b10110 << 11,
	POP  = 0b10111 << 11,
	CALL = 0b11000 << 11,
	RET  = 0b11001 << 11
} instruction;

typedef enum {
	R1 = 0b000,
	R2 = 1,
	R3 = 2,
	R4 = 3,
	R5 = 4,
	R6 = 5,
	R7 = 6,
	R8 = 7
} rx;

typedef struct {
	word pc;
	word sp;
	word memory[MEMORY_SIZE];
	word registers[REGISTER_COUNT];
	pid_t cpu_pid;
} cpu_t;

#endif
