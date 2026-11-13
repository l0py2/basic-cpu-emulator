#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "is.h"
#include "cpu-core.h"

#define CLOCK_RATE_MICS 10000

void setup_signal_handling();
void setup_shared_memory();

void usr1_handler(int signum);
void term_handler(int signum);

static cpu_t *cpu = NULL;
static int exit_program = 0;

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Usage: %s [binary path]\n", argv[0]);
		return 1;
	}

	setup_signal_handling();
	setup_shared_memory();

	setup_cpu(argv[1], cpu);

	while(!exit_program) {
		usleep(CLOCK_RATE_MICS);
		execute();
	}

	return 0;
}

void setup_signal_handling() {
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
}

void setup_shared_memory() {
	int cpu_fd = shm_open("/basic-cpu-emu", O_RDWR | O_CREAT, 0600);

	if(cpu_fd < 0) {
		printf("Failed to open shared memory object\n");
		exit(1);
	}

	if(ftruncate(cpu_fd, sizeof(cpu_t)) != 0) {
		printf("Failed to truncate shared memory\n");
		close(cpu_fd);
		exit(1);
	}

	cpu = mmap(NULL, sizeof(cpu_t), PROT_READ | PROT_WRITE, MAP_SHARED, cpu_fd, 0);
	close(cpu_fd);

	if(cpu == MAP_FAILED) {
		printf("Failed to map shared memory\n");
		shm_unlink("/basic-cpu-emu");
		exit(1);
	}

	cpu->cpu_pid = getpid();
}

void usr1_handler(int signum) {
}

void term_handler(int signum) {
	exit_program = 1;
}
