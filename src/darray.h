#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>

typedef struct darray {
    void *items;           // Begin of bufferb
    size_t item_size;      // Size of each item
    size_t item_count;     // Logical item count
    size_t item_capacity;  // LOGICAL capacity of items
} darray_t;

#define DARRAY_FREE_SPACE_BEGIN(DARRAY_PTR) ((DARRAY_PTR)->item_count * (DARRAY_PTR)->item_size)
#define DARRAY_BUFFER_CAPACITY_BYTES(DARRAY_PTR) ((DARRAY_PTR)->item_capacity * (DARRAY_PTR)->item_size)
#define DARRAY_FOREACH(DARRAY_PTR, TYPE, IT) \
    for (TYPE *IT = (DARRAY_PTR)->items; IT != ((TYPE *)(DARRAY_PTR)->items) + (DARRAY_PTR)->item_count; IT++)

/*
 * Dynamic array initalization function
 */
void darray_init(darray_t *out_darray, size_t item_capacity, size_t item_size);

/*
 * Copies item and pushes it at the and of buffer
 */
void darray_push(darray_t *darray, void *item);

/*
 * Just moving pointer of buffer end to the left by `item_size`. And copies to the `out_item` ptr last item
 * value
 */
void darray_pop(darray_t *darray, void *out_item);

/*
 * Assigning `out_ptr_item` to value of last item address
 */
void darray_peek(const darray_t *darray, void const **const out_ptr_item);

/*
 * Deallocates inner buffer and setting all stats except `item_size` to zero
 */
void darray_free(darray_t *darray);

#endif  // DARRAY_H