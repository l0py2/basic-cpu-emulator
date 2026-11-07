#ifndef CPU_CORE_H
#define CPU_CORE_H

#include "is.h"

void setup_cpu(char *path, cpu_t *new_cpu);
word extract_register(word instruction);
word extract_value(word instruction);
void execute();

#endif
