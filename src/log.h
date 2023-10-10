#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifndef __NO_LOGS__

#define LOG_PREFIX(FD, PREFIX, ...)      \
    do {                                 \
        fprintf(FD, PREFIX __VA_ARGS__); \
        putchar('\n');                   \
    } while (0)

#define LOG_DEBUG(...) LOG_PREFIX(stdout, "[DEBUG]: ", __VA_ARGS__)

#define LOG_INFO(...) LOG_PREFIX(stdout, "[INFO]: ", __VA_ARGS__)

#define LOG_ERROR(...) LOG_PREFIX(stderr, "[ERROR]: ", __VA_ARGS__)

#else

#define LOG_PREFIX(FD, PREFIX, ...)                 \
    do {                                            \
        (void)(FD);                                 \
        const char *__IGNORE = PREFIX #__VA_ARGS__; \
        (void)__IGNORE;                             \
    } while (0)

#define LOG_DEBUG(...) LOG_PREFIX(stdout, "[DEBUG]: ", __VA_ARGS__)

#define LOG_INFO(...) LOG_PREFIX(stdout, "[INFO]: ", __VA_ARGS__)

#define LOG_ERROR(...) LOG_PREFIX(stderr, "[ERROR]: ", __VA_ARGS__)

#endif  // __NO_LOGS__

#endif  // LOG_H