#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "is.h"
#include "cpu-ui.h"
#include "cpu-core.h"

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

	if(!setup_tui(cpu)) {
		shm_unlink("/basic-cpu-emu");
		return 1;
	}

	setup_cpu(argv[1], cpu);

	int key;
	unsigned int line = 0;

	while(!exit_program) {
		print_help_view();
		print_registers_view();
		print_memory_view(line);

		refresh_tui();

		key = getch();

		if(key == 'q') {
			break;
		}

		switch(key) {
			case 'c':
				execute();
				break;
			case 'j':
				line++;
				break;
			case 'k':
				line--;
				break;
			case 's':
				line = 0;
				break;
			case 'e':
				line = (MEMORY_SIZE / 16) - 1;
				break;
		}

		if(line > ((MEMORY_SIZE / 16) - 1)) {
			line = 0;
		}
	}

	end_tui();

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
