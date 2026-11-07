#ifndef IS_H
#define IS_H

#include <stdint.h>
#include <unistd.h>

#define MEMORY_SIZE 65536 // 2^16 addresses
#define REGISTER_COUNT 8

typedef uint16_t word;

typedef enum {
	NOP   = 0b00000 << 11,
	LDHI  = 1 << 11,
	LDLI  = 2 << 11,
	ADDI  = 3 << 11,
	RJUMP = 4 << 11
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
