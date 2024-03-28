#include <lt/mem.h>
#include <lt/math.h>
#include <lt/align.h>

#include <immintrin.h>

usz lt_mset16(void* dst, u16 v, usz size) {
	u8* it = dst, *end = dst + size;
	u8* end16 = (u8*)lt_align_bwd((usz)end, sizeof(u16));

	if ((usz)dst & 0b1) {
		*it++ = v;
		v = lt_rotr16(v, 8);
	}

	u8* avx2_start = (u8*)lt_align_fwd((usz)it, 32);
	u8* avx2_end = (u8*)lt_align_bwd((usz)end, 32);

	if ((isz)(avx2_end - avx2_start) < 32) {
		while (it < end16) {
			*(u16*)it = v;
			it += sizeof(u16);
		}
	}
	else {
		// align to 32 bytes
		while (it < avx2_start) {
			*(u16*)it = v;
			it += sizeof(u16);
		}

		__m256i ymm_v = _mm256_set1_epi16(v);
		while (it < avx2_end) {
			_mm256_store_si256((__m256i*)it, ymm_v);
			it += sizeof(__m256i);
		}

		// set unaligned bytes after avx_end
		while (it < end16) {
			*(u16*)it = v;
			it += sizeof(u16);
		}
	}
	// set 16-bit unaligned byte
	if ((usz)end & 0b1)
		*end = v;

	return size;
}

usz lt_mset32(void* dst, u32 v, usz size) {
	u8* it = dst, *end = it + size;
	u8* end32 = (u8*)lt_align_bwd((usz)end, sizeof(u32));

	usz unaligned32 = lt_align_fwd((usz)it, sizeof(u32)) - (usz)it;
	memcpy(it, &v, unaligned32);
	v = lt_rotr32(v, unaligned32 << 3);
	it += unaligned32;

	u8* avx2_start = (u8*)lt_align_fwd((usz)it, 32);
	u8* avx2_end = (u8*)lt_align_bwd((usz)end, 32);

	if ((isz)(avx2_end - avx2_start) < 64) {
		while (it < end32) {
			*(u32*)it = v;
			it += sizeof(u32);
		}
	}
	else {
		// align to 32 bytes
		while (it < avx2_start) {
			*(u32*)it = v;
			it += sizeof(u32);
		}

		__m256i ymm_v = _mm256_set1_epi32(v);
		while (it < avx2_end) {
			_mm256_store_si256((__m256i*)it, ymm_v);
			it += sizeof(__m256i);
		}

		// set unaligned bytes after avx_end
		while (it < end32) {
			*(u32*)it = v;
			it += sizeof(u32);
		}
	}

	// set 32-bit unaligned bytes
	memcpy(it, &v, end - end32);

	return size;
}
