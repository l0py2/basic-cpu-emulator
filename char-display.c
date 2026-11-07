#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "is.h"

#define REFRESH_RATE_MICS 100000

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

	for(;;) {
		usleep(REFRESH_RATE_MICS);
		printf("%c\n", (char) ((0x00ff) & cpu->memory[0]));
	}

	return 0;
}
