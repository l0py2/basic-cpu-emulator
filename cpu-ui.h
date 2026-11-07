#ifndef CPU_UI_H
#define CPU_UI_H

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

int setup_tui(cpu_t *new_cpu);
void refresh_tui();
void end_tui();
void print_help_view();
void print_registers_view();
void print_memory_view(unsigned int current_line);
void default_border(WINDOW *window);

#endif
