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

	for(unsigned int i = 0; i < MEMORY_SIZE; i++) {
		cpu->memory[i] = NOP;
	}

	for(word i = 0; i < REGISTER_COUNT; i++) {
		cpu->registers[i] = 0;
	}

	read_file(path);
}

word extract_register(word instruction) {
	return (instruction & 0x0700) >> 8;
}

word extract_value(word instruction) {
	return instruction & 0x00ff;
}

void execute() {
	word iregister, ivalue, temp = 0;

	iregister = extract_register(cpu->memory[cpu->pc]);
	ivalue = extract_value(cpu->memory[cpu->pc]);

	switch(cpu->memory[cpu->pc] & 0b11111 << 11) {
		case NOP:
			cpu->pc++;
			break;
		case LDHI:
			cpu->registers[iregister] =
				(ivalue << 8)
				| (cpu->registers[iregister] & 0x00ff);
			cpu->pc++;
			break;
		case LDLI:
			cpu->registers[iregister] =
				ivalue
				| (cpu->registers[iregister] & 0xff00);
			cpu->pc++;
			break;
		case ADDI:
			// Preserve signal
			if(ivalue & 0b10000000) {
				ivalue |= 0xff00;
			}

			cpu->registers[iregister] += ivalue;
			cpu->pc++;
			break;
		case RJUMP:
			if(ivalue & 0b10000000) {
				ivalue |= 0xff00;
			}

			cpu->pc += ivalue;
			break;
		case LDM:
			cpu->registers[iregister] = cpu->memory[cpu->registers[extract_register(ivalue << 8)]];
			cpu->pc++;
			break;
		case SVM:
			cpu->memory[cpu->registers[iregister]] = cpu->registers[extract_register(ivalue << 8)];
			cpu->pc++;
			break;
		default:
			printf("Unknown instruction: %4X\n", cpu->memory[cpu->pc]);
			cpu->pc++;
			break;
	}
}
