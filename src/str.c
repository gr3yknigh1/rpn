#include "str.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "memory.h"

str_t str_init(const char *src) { return str_init_0(src, strlen(src)); }

str_t str_init_0(const char *src, size_t src_len) {
    size_t src_size = src_len + 1;

    char *data = malloc(src_size);
    memory_copy(data, src, src_size);
    return (str_t){
        .data = data,
        .size = src_size,
    };
}

str_t str_init_1(const char *buf, size_t buf_size) {
    size_t src_size = buf_size + 1;

    char *data = malloc(src_size);
    memory_copy(data, buf, buf_size);
    data[buf_size] = '\0';

    return (str_t){
        .data = data,
        .size = src_size,
    };
}

str_t str_make_empty(void) {
    return (str_t){
        .data = NULL,
        .size = 0,
    };
}

void str_set(str_t *str, const str_t src) {
    if (str->data == NULL) {
        str->data = malloc(src.size);
    } else if (str->size < src.size) {
        str->data = realloc(str->data, src.size);
    }

    memory_copy(str->data, src.data, src.size);
    str->size = src.size;
}

void str_set_0(str_t *str, const char *src) {
    size_t src_size = strlen(src) + 1;

    if (str->data == NULL) {
        str->data = malloc(src_size);
    } else if (str->size < src_size) {
        str->data = realloc(str->data, src_size);
    }

    memory_copy(str->data, src, src_size);
    str->size = src_size;
}

void str_trim_char(char *out_buf, const char *str, char c) {
    size_t buf_idx = 0;

    while (*str != '\0') {
        if (*str != c) {
            out_buf[buf_idx] = *str;
            ++buf_idx;
        }
        ++str;
    }
    out_buf[buf_idx] = '\0';
}

bool str_is_equal(const str_t a, const str_t b) {
    bool is_equal = true;

    if (a.size != b.size) {
        is_equal = false;
    } else {
        is_equal = strcmp(a.data, b.data) == 0;
    }

    return is_equal;
}

bool str_is_equal_0(const str_t a, const char *b) { return strcmp(a.data, b) == 0; }

void str_free(str_t *str) {
    free(str->data);
    str->size = 0;
}
