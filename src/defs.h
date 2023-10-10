#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>

#include "log.h"

#define STRINGIFY(X) #X
#define STATIC_SIZE(X) (sizeof((X)) / sizeof((X)[0]))

#define ASSERT(EXPR)                                                                      \
    do {                                                                                  \
        if (!(EXPR)) {                                                                    \
            PANIK("Assertion error: '" STRINGIFY(EXPR) "' at " __FILE__ ":%i", __LINE__); \
        }                                                                                 \
    } while (0)

#define PANIK(...)                                \
    do {                                          \
        fprintf(stderr, "[PANIK]: " __VA_ARGS__); \
        putchar('\n');                            \
        exit(EXIT_FAILURE);                       \
    } while (0)

#endif  // DEFS_H