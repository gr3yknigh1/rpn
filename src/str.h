#ifndef STR_H
#define STR_H

#include <stdlib.h>

#include "bool.h"

typedef struct str {
    char *data;
    size_t size;
} str_t;

str_t str_init(const char *src);
str_t str_init_0(const char *src, size_t len);
str_t str_init_1(const char *buf, size_t size);
str_t str_make_empty(void);

void str_set(str_t *str, const str_t src);
void str_set_0(str_t *str, const char *src);

void str_trim_char(char *out_buf, const char *str, char c);

bool str_is_equal(const str_t a, const str_t b);
bool str_is_equal_0(const str_t a, const char *b);

void str_free(str_t *str);

#endif  // STR_H
