#include <lt/rle.h>
#include <lt/debug.h>
#include <lt/mem.h>

usz lt_rlencode(void* buf, usz size, void* out_) {
	u8* out = out_, *in = buf;

	u8* end = in + size;
	while (in < end) {
		u8 *vp = in++, v = *vp;
		while (in < end && *in == v) {
			++in;
		}
		usz repcount = in - vp;

		if LT_LIKELY(repcount <= 0x7F) {
			*out++ = repcount;
		}
		else if LT_LIKELY(repcount <= 0x3FFF) {
			*out++ = repcount | LT_RLE_EXT;
			*out++ = (repcount >> 7);
		}
		else if LT_LIKELY(repcount <= 0x1FFFFF) {
			*out++ = repcount | LT_RLE_EXT;
			*out++ = (repcount >> 7) | LT_RLE_EXT;
			*out++ = (repcount >> 14);
		}
		else if LT_LIKELY(repcount <= 0x0FFFFFFF) {
			*out++ = repcount | LT_RLE_EXT;
			*out++ = (repcount >> 7) | LT_RLE_EXT;
			*out++ = (repcount >> 14) | LT_RLE_EXT;
			*out++ = (repcount >> 21);
		}
		else {
			LT_ASSERT(repcount < ((usz)1 << 35));
			*out++ = repcount | LT_RLE_EXT;
			*out++ = (repcount >> 7) | LT_RLE_EXT;
			*out++ = (repcount >> 14) | LT_RLE_EXT;
			*out++ = (repcount >> 21) | LT_RLE_EXT;
			*out++ = (repcount >> 28);
		}
		lt_ierrf("%uz%c\n", repcount, v);
		*out++ = v;
	}

	return out - (u8*)out_;
}

usz lt_rldecode(void* buf, usz size, void* out_) {
	u8* in = buf, *end = in + size;
	u8* out = out_;
	while (in < end) {
		usz repcount = 0;

		u8 b;
		usz shift = 0;
		do {	b = *in++;
			repcount |= (b & LT_RLE_SIZE_MASK) << shift;
			shift += 7;
		} while (b & LT_RLE_EXT);

		b = *in++;
		out += lt_mset8(out, b, repcount);
		lt_ierrf("%uz%c\n", repcount, b);
	}

	return out - (u8*)out_;
}
