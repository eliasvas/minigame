#include "mAlloc.h"


uintptr_t align_fwd(uintptr_t ptr, size_t align) {
	uintptr_t p, a, modulo;

	ASSERT(is_pow2(align));

	p = ptr;
	a = (uintptr_t)align;
	modulo = p & (a-1);

	if (modulo != 0) {
		p += a - modulo;
	}
	return p;
}

void *marena_alloc_align(mArena *a, size_t size, size_t align) {
	uintptr_t curr_ptr = (uintptr_t)a->buf + (uintptr_t)a->curr_offset;
	uintptr_t offset = align_fwd(curr_ptr, align);
	offset -= (uintptr_t)a->buf;

	if (offset+size <= a->buf_len) {
		void *ptr = &a->buf[offset];
		a->prev_offset = offset;
		a->curr_offset = offset+size;

		MEMSET(ptr, 0, size);
		return ptr;
	}
	return NULL;
}

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

void *marena_alloc(mArena *a, size_t size) {
	return marena_alloc_align(a, size, DEFAULT_ALIGNMENT);
}

void marena_init(mArena *a, void *backing_buffer, size_t backing_buffer_length) {
	a->buf = (unsigned char *)backing_buffer;
	a->buf_len = backing_buffer_length;
	a->curr_offset = 0;
	a->prev_offset = 0;
}

void marena_free(mArena *a) {}

void marena_free_all(mArena *a) {
	a->curr_offset = 0;
	a->prev_offset = 0;
}


void *arena_resize_align(mArena *a, void *old_memory, size_t old_size, size_t new_size, size_t align) {
	u8 *old_mem = (u8*)old_memory;

	ASSERT(is_pow2(align));

	if (old_mem == NULL || old_size == 0) {
		return marena_alloc_align(a, new_size, align);
	} else if (a->buf <= old_mem && old_mem < a->buf+a->buf_len) {
		if (a->buf+a->prev_offset == old_mem) {
			a->curr_offset = a->prev_offset + new_size;
			if (new_size > old_size) {
				memset(&a->buf[a->curr_offset], 0, new_size-old_size);
			}
			return old_memory;
		} else {
			void *new_memory = marena_alloc_align(a, new_size, align);
			size_t copy_size = old_size < new_size ? old_size : new_size;
			MEMCPY(new_memory, old_memory, copy_size);
			return new_memory;
		}

	} else {
		ASSERT(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}

}

void *marena_resize(mArena *a, void *old_memory, size_t old_size, size_t new_size) {
	return arena_resize_align(a, old_memory, old_size, new_size, DEFAULT_ALIGNMENT);
}