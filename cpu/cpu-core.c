#include <stdio.h>

#include "cpu-core.h"
#include "is.h"

static cpu_t *cpu = NULL;

void read_file(char *path) {
	FILE *file = fopen(path, "rb");

	if(file == NULL) {
		return;
	}

	word memory_word = 0;
	word current_address = 0;

	while(fread(&memory_word, 1, sizeof(word), file) && current_address < MEMORY_SIZE) {
		cpu->memory[current_address] = memory_word;
		current_address++;
	}

	fclose(file);
}

void setup_cpu(char *path, cpu_t *new_cpu) {
	cpu = new_cpu;

	cpu->pc = 0;
	cpu->sp = 0xffff;

	for(unsigned int i = 0; i < MEMORY_SIZE; i++) {
		cpu->memory[i] = NOP;
	}

	for(word i = 0; i < REGISTER_COUNT; i++) {
		cpu->registers[i] = 0;
	}

	cpu->cycle_signal = 0;
	cpu->instructions_per_cycle = 0;

	read_file(path);
}

void execute() {
	word iregister1, iregister2, iregister3;
	word ivalue;

	ivalue = cpu->memory[cpu->pc] & 0xff;
	iregister1 = (cpu->memory[cpu->pc] >> 8) & 0b111;
	iregister2 = ivalue & 0b111;
	iregister3 = (ivalue >> 3) & 0b111;

	switch(cpu->memory[cpu->pc] & (0b11111 << 11)) {
		case NOP:
			cpu->pc++;
			break;
		case LDHI:
			cpu->registers[iregister1] = (ivalue << 8) | (cpu->registers[iregister1] & 0xff);
			cpu->pc++;
			break;
		case LDLI:
			cpu->registers[iregister1] = ivalue | (cpu->registers[iregister1] & 0xff00);
			cpu->pc++;
			break;
		case ADD:
			cpu->registers[iregister1] = cpu->registers[iregister2] + cpu->registers[iregister3];
			cpu->pc++;
			break;
		case SUB:
			cpu->registers[iregister1] = cpu->registers[iregister2] - cpu->registers[iregister3];
			cpu->pc++;
			break;
		case NEG:
			cpu->registers[iregister1] = -cpu->registers[iregister2];
			cpu->pc++;
			break;
		case MULT:
			cpu->registers[iregister1] = cpu->registers[iregister2] * cpu->registers[iregister3];
			cpu->pc++;
			break;
		case DIV:
			cpu->registers[iregister1] = cpu->registers[iregister2] / cpu->registers[iregister3];
			cpu->pc++;
			break;
		case REM:
			cpu->registers[iregister1] = cpu->registers[iregister2] % cpu->registers[iregister3];
			cpu->pc++;
			break;
		case NOT:
			cpu->registers[iregister1] = ~cpu->registers[iregister2];
			cpu->pc++;
			break;
		case AND:
			cpu->registers[iregister1] = cpu->registers[iregister2] & cpu->registers[iregister3];
			cpu->pc++;
			break;
		case OR:
			cpu->registers[iregister1] = cpu->registers[iregister2] | cpu->registers[iregister3];
			cpu->pc++;
			break;
		case XOR:
			cpu->registers[iregister1] = cpu->registers[iregister2] ^ cpu->registers[iregister3];
			cpu->pc++;
			break;
		case SL:
			cpu->registers[iregister1] = cpu->registers[iregister2] << cpu->registers[iregister3];
			cpu->pc++;
			break;
		case SR:
			cpu->registers[iregister1] = cpu->registers[iregister2] >> cpu->registers[iregister3];
			cpu->pc++;
			break;
		case JUMP:
			cpu->pc = cpu->registers[iregister1];
			break;
		case BREQ:
			if(cpu->registers[iregister2] == cpu->registers[iregister3]) {
				cpu->pc = cpu->registers[iregister1];
			} else {
				cpu->pc++;
			}
			break;
		case BRNE:
			if(cpu->registers[iregister2] != cpu->registers[iregister3]) {
				cpu->pc = cpu->registers[iregister1];
			} else {
				cpu->pc++;
			}
			break;
		case BRGT:
			if(cpu->registers[iregister2] > cpu->registers[iregister3]) {
				cpu->pc = cpu->registers[iregister1];
			} else {
				cpu->pc++;
			}
			break;
		case BRLT:
			if(cpu->registers[iregister2] < cpu->registers[iregister3]) {
				cpu->pc = cpu->registers[iregister1];
			} else {
				cpu->pc++;
			}
			break;
		case LDM:
			cpu->registers[iregister1] = cpu->memory[cpu->registers[iregister2]];
			cpu->pc++;
			break;
		case SVM:
			cpu->memory[cpu->registers[iregister2]] = cpu->registers[iregister1];
			cpu->pc++;
			break;
		case PUSH:
			cpu->memory[cpu->sp] = cpu->registers[iregister1];
			cpu->sp--;
			cpu->pc++;
			break;
		case POP:
			cpu->sp++;
			cpu->registers[iregister1] = cpu->memory[cpu->sp];
			cpu->pc++;
			break;
		case CALL:
			cpu->memory[cpu->sp] = cpu->pc + 1;
			cpu->sp--;
			cpu->pc = cpu->registers[iregister1];
			break;
		case RET:
			cpu->sp++;
			cpu->pc = cpu->memory[cpu->sp];
			break;
		default:
			printf("Unknown instruction: %4X\n", cpu->memory[cpu->pc]);
			cpu->pc++;
			break;
	}
}
