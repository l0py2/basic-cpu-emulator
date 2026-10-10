#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "is.h"

void read_file(char *path);
word extract_register(word instruction);
word extract_value(word instruction);
void execute();
void usr1_handler(int signum);
void term_handler(int signum);

static cpu_t *cpu = NULL;
static int exit_program = 0;

int main(int argc, char **argv) {
	struct sigaction sig_action;

	sig_action.sa_handler = usr1_handler;
	sig_action.sa_flags = 0;
	sigemptyset(&(sig_action.sa_mask));
	sigaction(SIGUSR1, &sig_action, NULL);

	sig_action.sa_handler = term_handler;
	sig_action.sa_flags = 0;
	sigemptyset(&(sig_action.sa_mask));
	sigaction(SIGTERM, &sig_action, NULL);
	sigaction(SIGINT, &sig_action, NULL);

	int cpu_fd = shm_open("/basic-cpu-emu", O_RDWR | O_CREAT, 0600);

	if(cpu_fd < 0) {
		printf("Failed to open shared memory object\n");
		return 1;
	}

	if(ftruncate(cpu_fd, sizeof(cpu_t)) != 0) {
		printf("Failed to truncate shared memory\n");
		close(cpu_fd);
		return 1;
	}

	cpu = mmap(NULL, sizeof(cpu_t), PROT_READ | PROT_WRITE, MAP_SHARED, cpu_fd, 0);
	close(cpu_fd);

	if(cpu == MAP_FAILED) {
		printf("Failed to map shared memory\n");
		shm_unlink("/basic-cpu-emu");
		return 1;
	}

	cpu->cpu_pid = getpid();

	if(argc < 2) {
		printf("Usage: %s [binary path]\n", argv[0]);
		shm_unlink("/basic-cpu-emu");
		return 1;
	}

	for(unsigned int i = 0; i < MEMORY_SIZE; i++) {
		cpu->memory[i] = NOP;
	}

	for(word i = 0; i < REGISTER_COUNT; i++) {
		cpu->registers[i] = 0;
	}

	read_file(argv[1]);

	int key;
	unsigned int line = 0;

	while(!exit_program) {
		pause();
	}

	shm_unlink("/basic-cpu-emu");

	return 0;
}

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

	switch(cpu->memory[cpu->pc] >> 11) {
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
		case LDM:
			cpu->registers[iregister] = cpu->memory[cpu->registers[extract_register(ivalue << 8)]];
			cpu->pc++;
			break;
		case SVM:
			cpu->memory[cpu->registers[extract_register(ivalue << 8)]] = cpu->registers[iregister];
			cpu->pc++;
			break;
		case COPY:
			cpu->registers[iregister] = cpu->registers[extract_register(ivalue << 8)];
			cpu->pc++;
			break;
		case ADDI:
			cpu->registers[iregister] += ivalue;
			cpu->pc++;
			break;
		case SUBI:
			cpu->registers[iregister] -= ivalue;
			cpu->pc++;
			break;
		case COMPI:
			temp = cpu->registers[iregister] - ivalue;

			cpu->registers[FLAGS] = cpu->registers[FLAGS] & ~(ZERO | OVERFLOW);

			if(temp == 0) {
				cpu->registers[FLAGS] = cpu->registers[FLAGS] | ZERO;
			}

			if(temp > cpu->registers[iregister]) {
				cpu->registers[FLAGS] = cpu->registers[FLAGS] | OVERFLOW;
			}

			cpu->pc++;
			break;
		case SLI:
			cpu->registers[iregister] = cpu->registers[iregister] << ivalue;
			cpu->pc++;
			break;
		case SRI:
			cpu->registers[iregister] = cpu->registers[iregister] >> ivalue;
			cpu->pc++;
			break;
		case CLR:
			cpu->registers[iregister] = 0x0000;
			cpu->pc++;
			break;
		case SET:
			cpu->registers[iregister] = 0xffff;
			cpu->pc++;
			break;
		case NOT:
			cpu->registers[iregister] = ~cpu->registers[iregister];
			cpu->pc++;
			break;
		case ADD:
			cpu->registers[iregister] += cpu->registers[extract_register(ivalue << 8)];
			cpu->pc++;
			break;
		case SUB:
			cpu->registers[iregister] -= cpu->registers[extract_register(ivalue << 8)];
			cpu->pc++;
			break;
		case COMP:
			temp = cpu->registers[iregister]
				- cpu->registers[extract_register(ivalue << 8)];

			cpu->registers[FLAGS] = cpu->registers[FLAGS] & ~(ZERO | OVERFLOW);

			if(temp == 0) {
				cpu->registers[FLAGS] = cpu->registers[FLAGS] | ZERO;
			}

			if(temp > cpu->registers[iregister]) {
				cpu->registers[FLAGS] = cpu->registers[FLAGS] | OVERFLOW;
			}

			cpu->pc++;
			break;
		case AND:
			cpu->registers[iregister] &= cpu->registers[extract_register(ivalue << 8)];
			cpu->pc++;
			break;
		case OR:
			cpu->registers[iregister] |= cpu->registers[extract_register(ivalue << 8)];
			cpu->pc++;
			break;
		case JUMP:
			cpu->pc = ivalue;
			break;
		case BRNEQ:
			if(cpu->registers[FLAGS] & ZERO) {
				cpu->pc++;
			} else {
				cpu->pc = ivalue;
			}
			break;
		case BREQ:
			if(cpu->registers[FLAGS] & ZERO) {
				cpu->pc = ivalue;
			} else {
				cpu->pc++;
			}
			break;
		case CALL:
			cpu->memory[cpu->sp] = cpu->pc + 1;
			cpu->sp--;
			cpu->pc = ivalue;
			break;
		case PUSH:
			cpu->memory[cpu->sp] = cpu->registers[iregister];
			cpu->sp--;
			cpu->pc++;
			break;
		case POP:
			cpu->sp++;
			cpu->registers[iregister] = cpu->memory[cpu->sp];
			cpu->pc++;
			break;
		case RET:
			cpu->sp++;
			cpu->pc = cpu->memory[cpu->sp];
			break;
		default:
			cpu->pc++;
			break;
	}
}

void usr1_handler(int signum) {
	execute();
}

void term_handler(int signum) {
	exit_program = 1;
}
