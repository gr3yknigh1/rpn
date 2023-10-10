#include "memory.h"

void memory_copy(void *restrict dest, const void *src, size_t size) {
    char *dest_it = dest;
    const char *src_it = src;
    while (size > 0) {
        *dest_it = *src_it;
        ++dest_it;
        ++src_it;
        --size;
    }
}