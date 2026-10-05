#ifndef TUI_H
#define TUI_H

#include <ncurses.h>

#include "is.h"

#define HELP_WINDOW_LINES 4
#define REGISTERS_WINDOW_LINES 5
#define MIN_MEMORY_WINDOW_LINES 10

static int term_max_lines = 0;
static int term_max_cols = 0;
static int term_rel_lines = 0;

static WINDOW *help_view;
static WINDOW *registers_view;
static WINDOW *memory_view;

int initialize_tui();
void refresh_tui();
void end_tui();
void print_help_view();
void print_registers_view(word pc, word sp, word *registers, word register_count);
void print_memory_view(word *memory, unsigned int memory_size, unsigned int current_line);
void default_border(WINDOW *window);

#endif
