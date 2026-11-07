#ifndef CPU_CORE_H
#define CPU_CORE_H

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

void setup_cpu(char *path, cpu_t *new_cpu);
word extract_register(word instruction);
word extract_value(word instruction);
void execute();

#endif
