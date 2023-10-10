#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

void memory_copy(void *restrict dest, const void *src, size_t size);

#endif  // MEMORY_H