#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "is.h"

typedef enum {
	VGA_BLACK = 0,
	VGA_BLUE = 1,
	VGA_GREEN = 2,
	VGA_CYAN = 3,
	VGA_RED = 4,
	VGA_MAGENTA = 5,
	VGA_BROWN = 6,
	VGA_LIGHT_GRAY = 7,
	VGA_DARK_GRAY = 8,
	VGA_LIGHT_BLUE = 9,
	VGA_LIGHT_GREEN = 10,
	VGA_LIGHT_CYAN = 11,
	VGA_LIGHT_RED = 12,
	VGA_LIGHT_MAGENTA = 13,
	VGA_YELLOW = 14,
	VGA_WHITE = 15
} vga_colour;

typedef enum {
	ASCII_FG_BLACK = 30,
	ASCII_BG_BLACK = 40,
	ASCII_FG_RED = 31,
	ASCII_BG_RED = 41,
	ASCII_FG_GREEN = 32,
	ASCII_BG_GREEN = 42,
	ASCII_FG_YELLOW = 33,
	ASCII_BG_YELLOW = 43,
	ASCII_FG_BLUE = 34,
	ASCII_BG_BLUE = 44,
	ASCII_FG_MAGENTA = 35,
	ASCII_BG_MAGENTA = 45,
	ASCII_FG_CYAN = 36,
	ASCII_BG_CYAN = 46,
	ASCII_FG_WHITE = 37,
	ASCII_BG_WHITE = 47
} ascii_colour;

static cpu_t *cpu = NULL;

int vga_colour_to_fg_ascii(word vga_text_word);
int vga_colour_to_bg_ascii(word vga_text_word);
void print_char(word vga_text_word);

int main(void) {
	int cpu_fd = shm_open("/basic-cpu-emu", O_RDONLY, 0);

	if(cpu_fd < 0) {
		printf("Failed to open shared memory object\n");
		return 1;
	}

	cpu = mmap(NULL, sizeof(cpu_t), PROT_READ, MAP_SHARED, cpu_fd, 0);
	close(cpu_fd);

	if(cpu == MAP_FAILED) {
		printf("Failed to map shared memory\n");
		return 1;
	}

	print_char(cpu->memory[0]);
	printf("\n");

	return 0;
}

int vga_colour_to_fg_ascii(word vga_text_word) {
	switch((vga_text_word >> 8) & 0x0f) {
		case VGA_BLACK:
			return ASCII_FG_BLACK;
		case VGA_GREEN:
			return ASCII_FG_GREEN;
		case VGA_CYAN:
			return ASCII_FG_CYAN;
		case VGA_RED:
			return ASCII_FG_RED;
		case VGA_MAGENTA:
			return ASCII_FG_MAGENTA;
		case VGA_BROWN:
			return ASCII_FG_BLACK;
		default:
			return ASCII_FG_WHITE;
	}
}

int vga_colour_to_bg_ascii(word vga_text_word) {
	switch((vga_text_word >> 12) & 0b0111) {
		case VGA_BLACK:
			return ASCII_BG_BLACK;
		case VGA_GREEN:
			return ASCII_BG_GREEN;
		case VGA_CYAN:
			return ASCII_BG_CYAN;
		case VGA_RED:
			return ASCII_BG_RED;
		case VGA_MAGENTA:
			return ASCII_BG_MAGENTA;
		case VGA_BROWN:
			return ASCII_BG_BLACK;
		default:
			return ASCII_BG_BLACK;
	}
}

void print_char(word vga_text_word) {
	char c = vga_text_word & 0x00ff;
	int fg_colour = vga_colour_to_fg_ascii(vga_text_word);
	int bg_colour = vga_colour_to_bg_ascii(vga_text_word);

	printf("\033[%d;%dm%c\033[0m", fg_colour, bg_colour, c);
}
