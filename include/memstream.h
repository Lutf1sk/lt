#pragma once

#include <lt2/common.h>

typedef struct memstream {
	u8* data;
	u8* it;
	u8* end;
	b8 failed;
} memstream_t;

INLINE
void ms_write8(memstream_t* ms, u8 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end)
		ms->failed = 1;
	else
		*ms->it++ = v;
}

INLINE
void ms_write16(memstream_t* ms, u16 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end) {
		ms->failed = 1;
		return;
	}
	memcpy(ms->it, &v, sizeof(v));
	ms->it += sizeof(v);
}

INLINE
void ms_write32(memstream_t* ms, u32 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end) {
		ms->failed = 1;
		return;
	}
	memcpy(ms->it, &v, sizeof(v));
	ms->it += sizeof(v);
}

INLINE
void ms_write64(memstream_t* ms, u64 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end) {
		ms->failed = 1;
		return;
	}
	memcpy(ms->it, &v, sizeof(v));
	ms->it += sizeof(v);
}

INLINE
void ms_writes(memstream_t* ms, ls v) {
	if UNLIKELY (ms->it + v.size > ms->end) {
		ms->failed = 1;
		return;
	}
	memcpy(ms->it, v.ptr, v.size);
	ms->it += v.size;
}

INLINE
void ms_write(memstream_t* ms, void* data, usz size) {
	if UNLIKELY (ms->it + size > ms->end) {
		ms->failed = 1;
		return;
	}
	memcpy(ms->it, data, size);
	ms->it += size;
}

INLINE
ls ms_result(memstream_t* ms) {
	if UNLIKELY (ms->failed)
		return ls("");
	return lls(ms->data, ms->it - ms->data);
}

