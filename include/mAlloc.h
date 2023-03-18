#ifndef MALLOC_H
#define MALLOC_H
#include "base.h"
//this is gooooood https://www.gingerbill.org/series/memory-allocation-strategies/

typedef struct {
	u8 *buf;
	u32 buf_len;
	u32 prev_offset;
	u32 curr_offset;
}mArena;

void marena_init(mArena *a, void *backing_buffer, size_t backing_buffer_length);
void *marena_alloc(mArena *a, size_t size);
void *marena_resize(mArena *a, void *old_memory, size_t old_size, size_t new_size);
void marena_free(mArena *a);

#endif