#pragma once

#include <lt2/common.h>

typedef struct memstream {
	u8* data;
	u8* it;
	u8* end;
} memstream_t;

INLINE
b8 ms_write8(memstream_t ms[static 1], u8 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end)
		return 1;
	*ms->it++ = v;
	return 0;
}

INLINE
b8 ms_write16(memstream_t ms[static 1], u16 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end)
		return 1;
	memcpy(ms->it, &v, sizeof(v));
	ms->it += sizeof(v);
	return 0;
}

INLINE
b8 ms_write32(memstream_t ms[static 1], u32 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end)
		return 1;
	memcpy(ms->it, &v, sizeof(v));
	ms->it += sizeof(v);
	return 0;
}

INLINE
b8 ms_write64(memstream_t ms[static 1], u64 v) {
	if UNLIKELY (ms->it + sizeof(v) > ms->end)
		return 1;
	memcpy(ms->it, &v, sizeof(v));
	ms->it += sizeof(v);
	return 0;
}

INLINE
b8 ms_writes(memstream_t ms[static 1], ls v) {
	if UNLIKELY (ms->it + v.size > ms->end)
		return 1;
	memcpy(ms->it, v.ptr, v.size);
	ms->it += v.size;
	return 0;
}

INLINE
b8 ms_write(memstream_t ms[static 1], void* data, usz size) {
	if UNLIKELY (ms->it + size > ms->end)
		return 1;
	memcpy(ms->it, data, size);
	ms->it += size;
	return 0;
}

INLINE
ls ms_result(memstream_t ms[static 1]) {
	return lls(ms->data, ms->it - ms->data);
}

