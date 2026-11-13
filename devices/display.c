#include <raylib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../cpu/is.h"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400
#define PIXEL_SIZE 40

#define DISPLAY_ROWS (SCREEN_HEIGHT / PIXEL_SIZE)
#define DISPLAY_COLS (SCREEN_WIDTH / PIXEL_SIZE)

void read_display(word display[DISPLAY_ROWS][DISPLAY_COLS]);
void draw_pixels(word display[DISPLAY_ROWS][DISPLAY_COLS]);

static cpu_t *cpu = NULL;

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

	word display[DISPLAY_ROWS][DISPLAY_COLS] = { 0 };

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Display");
	SetTargetFPS(10);

	while(!WindowShouldClose()) {
		read_display(display);

		BeginDrawing();

			ClearBackground(BLACK);
			draw_pixels(display);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

void read_display(word display[DISPLAY_ROWS][DISPLAY_COLS]) {
	for(int i = 0; i < DISPLAY_ROWS; i++) {
		for(int j = 0; j < DISPLAY_COLS; j++) {
			display[i][j] = cpu->memory[j + (i * DISPLAY_COLS)];
		}
	}
}

void draw_pixels(word display[DISPLAY_ROWS][DISPLAY_COLS]) {
	for(int i = 0; i < DISPLAY_ROWS; i++) {
		for(int j = 0; j < DISPLAY_COLS; j++) {
			Color color = {
				(display[i][j] & (0b11111 << 10) >> 10) * 255 / 0b11111,
				(display[i][j] & (0b11111 << 5) >> 5) * 255 / 0b11111,
				display[i][j] & 0b11111 * 255 / 0b11111,
				0xff
			};

			DrawRectangle(j * PIXEL_SIZE, i * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE, color);
		}
	}
}
