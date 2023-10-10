#include "darray.h"

#include "memory.h"

void darray_init(darray_t *out_darray, size_t item_capacity, size_t item_size) {
    *out_darray = (darray_t){
        .items = malloc(item_size * item_capacity),
        .item_size = item_size,
        .item_count = 0,
        .item_capacity = item_capacity,
    };
}
void darray_push(darray_t *darray, void *item) {
    if (darray->item_count + 1 >= darray->item_capacity) {
        size_t new_capacity = DARRAY_BUFFER_CAPACITY_BYTES(darray) * 2;
        darray->items = realloc(darray->items, new_capacity);
        darray->item_capacity *= 2;
    }

    size_t offset = DARRAY_FREE_SPACE_BEGIN(darray);
    memory_copy(((char *)darray->items) + offset, item, darray->item_size);
    darray->item_count += 1;
}

void darray_pop(darray_t *darray, void *out_item) {
    if (darray->item_count == 0) {
        return;
    }
    darray->item_count -= 1;

    if (out_item != NULL) {
        memory_copy(out_item, ((char *)darray->items) + darray->item_count * darray->item_size,
                    darray->item_size);
    }
}

void darray_peek(const darray_t *darray, void const **const out_ptr_item) {
    if (darray->item_count == 0) {
        return;
    }
    size_t offset = DARRAY_FREE_SPACE_BEGIN(darray);
    *out_ptr_item = darray + offset - darray->item_size;
    //                                ^^^^^^^^^^^^^^^^^ - Substract size of last item
}

void darray_free(darray_t *darray) {
    free(darray->items);
    darray->items = NULL;
    darray->item_count = 0;
    darray->item_capacity = 0;
}
