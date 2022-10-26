#include <lt/io.h>
#include <lt/mem.h>

#define INITIAL_SIZE 32

typedef
struct lt_io_alloc_ctx {
	lt_alloc_t* alc;
	char* str;
	usz len;
	usz asize;
} lt_io_alloc_ctx_t;

isz lt_alloc_io_callb(lt_io_alloc_ctx_t* cx, void* data, usz len) {
	if (!len)
		return len;

	if (cx->len + len > cx->asize) {
		while (cx->len + len > cx->asize)
			cx->asize <<= 1;

		cx->str = lt_mrealloc(cx->alc, cx->str, cx->asize); // !!
	}

	memcpy(cx->str + cx->len, data, len);
	cx->len += len;
	return len;
}

isz lt_vaprintf(lstr_t* out, lt_alloc_t* alc, char* fmt, va_list argl) {
	lt_io_alloc_ctx_t ctx;
	ctx.alc = alc;
	ctx.str = lt_malloc(alc, INITIAL_SIZE);
	ctx.len = 0;
	ctx.asize = INITIAL_SIZE;
	if (!ctx.str)
		goto err0;

	isz res = lt_io_vprintf((lt_io_callback_t)lt_alloc_io_callb, &ctx, fmt, argl);
	if (res < 0)
		goto err1;

	*out = LSTR(ctx.str, ctx.len);
	return res;

err1:	lt_mfree(alc, ctx.str);
err0:	*out = NLSTR();
		return -1;
}

LT_FLATTEN
isz lt_aprintf(lstr_t* out, lt_alloc_t* alc, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);
	isz bytes = lt_vaprintf(out, alc, fmt, argl);
	va_end(argl);
	return bytes;
}

