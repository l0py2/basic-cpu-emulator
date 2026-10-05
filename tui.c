#include "tui.h"

int initialize_tui() {
	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	getmaxyx(stdscr, term_max_lines, term_max_cols);

	if(term_max_lines < HELP_WINDOW_LINES + REGISTERS_WINDOW_LINES + MIN_MEMORY_WINDOW_LINES) {
		endwin();
		printf("Not enough lines in the terminal\n");
		return 0;
	}

	help_view = newwin(HELP_WINDOW_LINES, term_max_cols, 0, 0);
	term_rel_lines += HELP_WINDOW_LINES;

	if(help_view == NULL) {
		endwin();
		printf("Failed to initialize help window\n");
		return 0;
	}

	registers_view = newwin(REGISTERS_WINDOW_LINES, term_max_cols, term_rel_lines, 0);
	term_rel_lines += REGISTERS_WINDOW_LINES;

	if(registers_view == NULL) {
		endwin();
		printf("Failed to initialize registers window\n");
		return 0;
	}

	memory_view = newwin(term_max_lines - term_rel_lines, term_max_cols, term_rel_lines, 0);

	if(memory_view == NULL) {
		endwin();
		printf("Failed to initialize memory window\n");
		return 0;
	}

	return 1;
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

void print_registers_view(word pc, word sp, word *registers, word register_count) {
	mvwprintw(registers_view, 1, 1, "- REGISTERS -");
	mvwprintw(registers_view, 2, 1, "PC=%04X SP=%04X", pc, sp);
	wmove(registers_view, 3, 1);

	for(word i = 0; i < register_count; i++) {
		wprintw(registers_view, "R%d=%04X ", i + 1, registers[i]);
	}

	default_border(registers_view);
}

void print_memory_view(word *memory, unsigned int memory_size, unsigned int current_line) {
	wclear(memory_view);
	mvwprintw(memory_view, 1, 1, "- Memory (%d/%d line) -", current_line, (memory_size / 16) - 1);
	wmove(memory_view, 2, 1);

	int break_count = 2;
	int line_break = 1;

	unsigned int max_lines = term_max_lines - term_rel_lines - 2;
	unsigned int max_word = max_lines * 16;

	max_word = max_word + current_line * 16;

	if(max_word > memory_size) {
		max_word = memory_size;
	}

	for(unsigned int i = current_line * 16; i < max_word; i++) {
		wprintw(memory_view, "%04X ", memory[i]);

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
