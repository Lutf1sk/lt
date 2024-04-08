#include <lt/io.h>
#include <lt/mem.h>

#define INITIAL_SIZE 32

typedef
struct lt_io_alloc_ctx {
	lt_alloc_t* alloc;
	char* str;
	usz len;
	usz asize;
} lt_io_alloc_ctx_t;

isz lt_alloc_io_callb(lt_io_alloc_ctx_t* cx, const void* data, usz len) {
	if (!len)
		return len;

	if (cx->len + len > cx->asize) {
		while (cx->len + len > cx->asize)
			cx->asize <<= 1;

		void* res = lt_mrealloc(cx->alloc, cx->str, cx->asize);
		if (!res)
			return -LT_ERR_OUT_OF_MEMORY;
		cx->str = res;
	}

	memcpy(cx->str + cx->len, data, len);
	cx->len += len;
	return len;
}

isz lt_vaprintf(lstr_t out[static 1], lt_alloc_t alloc[static 1], const char* fmt, va_list argl) {
	lt_io_alloc_ctx_t ctx;
	ctx.alloc = alloc;
	ctx.str = lt_malloc(alloc, INITIAL_SIZE);
	ctx.len = 0;
	ctx.asize = INITIAL_SIZE;

	if (!ctx.str)
		return -LT_ERR_OUT_OF_MEMORY;

	isz res = lt_io_vprintf((lt_write_fn_t)lt_alloc_io_callb, &ctx, fmt, argl);
	if (res < 0) {
		lt_mfree(alloc, ctx.str);
		return res;
	}

	*out = LSTR(ctx.str, ctx.len);
	return res;
}

LT_FLATTEN
isz lt_aprintf(lstr_t out[static 1], lt_alloc_t alloc[static 1], const char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz res = lt_vaprintf(out, alloc, fmt, argl);
	va_end(argl);
	return res;
}
