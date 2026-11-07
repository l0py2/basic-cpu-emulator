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

#define HELP_WINDOW_LINES 4
#define REGISTERS_WINDOW_LINES 5
#define MIN_MEMORY_WINDOW_LINES 10

void setup_signal_handling();
void setup_shared_memory();
void setup_tui();

void read_file(char *path);
word extract_register(word instruction);
word extract_value(word instruction);
void execute();
void usr1_handler(int signum);
void term_handler(int signum);

void refresh_tui();
void end_tui();
void print_help_view();
void print_registers_view();
void print_memory_view(unsigned int current_line);
void default_border(WINDOW *window);

static cpu_t *cpu = NULL;
static int exit_program = 0;

static int term_max_lines = 0;
static int term_max_cols = 0;
static int term_rel_lines = 0;

static WINDOW *help_view;
static WINDOW *registers_view;
static WINDOW *memory_view;

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Usage: %s [binary path]\n", argv[0]);
		return 1;
	}

	setup_signal_handling();
	setup_tui();
	setup_shared_memory();

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

	shm_unlink("/basic-cpu-emu");

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

void setup_tui() {
	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	getmaxyx(stdscr, term_max_lines, term_max_cols);

	if(term_max_lines < HELP_WINDOW_LINES + REGISTERS_WINDOW_LINES + MIN_MEMORY_WINDOW_LINES) {
		endwin();
		printf("Not enough lines in the terminal\n");
		exit(1);
	}

	help_view = newwin(HELP_WINDOW_LINES, term_max_cols, 0, 0);
	term_rel_lines += HELP_WINDOW_LINES;

	if(help_view == NULL) {
		endwin();
		printf("Failed to initialize help window\n");
		exit(1);
	}

	registers_view = newwin(REGISTERS_WINDOW_LINES, term_max_cols, term_rel_lines, 0);
	term_rel_lines += REGISTERS_WINDOW_LINES;

	if(registers_view == NULL) {
		endwin();
		printf("Failed to initialize registers window\n");
		exit(1);
	}

	memory_view = newwin(term_max_lines - term_rel_lines, term_max_cols, term_rel_lines, 0);

	if(memory_view == NULL) {
		endwin();
		printf("Failed to initialize memory window\n");
		exit(1);
	}
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
		default:
			printf("Unknown instruction: %4X\n", cpu->memory[cpu->pc]);
			cpu->pc++;
			break;
	}
}

void usr1_handler(int signum) {}

void term_handler(int signum) {
	exit_program = 1;
}


void refresh_tui() {
	refresh();
	wrefresh(help_view);
	wrefresh(registers_view);
	wrefresh(memory_view);
}

void end_tui() {
	delwin(registers_view);
	delwin(memory_view);
	endwin();
}

void print_help_view() {
	mvwprintw(help_view, 1, 1, "- HELP -");
	mvwprintw(help_view, 2, 1, "q - quit   c - cycle   j/k - move between lines   s/e - goto start/end");

	default_border(help_view);
}

void print_registers_view() {
	mvwprintw(registers_view, 1, 1, "- REGISTERS -");
	mvwprintw(registers_view, 2, 1, "PC=%04X SP=%04X", cpu->pc, cpu->sp);
	wmove(registers_view, 3, 1);

	for(word i = 0; i < REGISTER_COUNT; i++) {
		wprintw(registers_view, "R%d=%04X ", i + 1, cpu->registers[i]);
	}

	default_border(registers_view);
}

void print_memory_view(unsigned int current_line) {
	wclear(memory_view);
	mvwprintw(memory_view, 1, 1, "- Memory (%d/%d line) -", current_line, (MEMORY_SIZE / 16) - 1);
	wmove(memory_view, 2, 1);

	int break_count = 2;
	int line_break = 1;

	unsigned int max_lines = term_max_lines - term_rel_lines - 2;
	unsigned int max_word = max_lines * 16;

	max_word = max_word + current_line * 16;

	if(max_word > MEMORY_SIZE) {
		max_word = MEMORY_SIZE;
	}

	for(unsigned int i = current_line * 16; i < max_word; i++) {
		wprintw(memory_view, "%04X ", cpu->memory[i]);

		if(line_break > 15) {
			line_break = 1;
			break_count++;
			wmove(memory_view, break_count, 1);
		} else {
			line_break++;
		}
	}

	default_border(memory_view);
}

void default_border(WINDOW *window) {
	wborder(window, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0');
}
