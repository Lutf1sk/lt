#include <lt2/common.h>
#include <lt2/bits.h>

#ifdef ON_AMD64

#include <immintrin.h>

static
void memset32_avx2(void* dst_, u32 v32, usz count) {
	__m256i v256 = _mm256_set1_epi32(v32);
	__m256i* it  = dst_;

	// 32 align
	if ((usz)it & 31) {
		_mm256_storeu_si256(dst_, v256);
		it = alignptr(dst_, 32);
		count -= ((void*)it - dst_) / sizeof(u32);
	}
	it = __builtin_assume_aligned(it, 32);

	//simd-aligned wide stores
	__m256i* end = it + (count >> 3);
	__m256i* unrolled_end = end - 4;
	asm volatile(".p2align 4");
	while (it <= unrolled_end) {
		_mm256_store_si256(it + 0, v256);
		_mm256_store_si256(it + 1, v256);
		_mm256_store_si256(it + 2, v256);
		_mm256_store_si256(it + 3, v256);
		it += 4;
	}

	switch (end - it) {
		case 7: _mm256_store_si256(it + 6, v256);
		case 6: _mm256_store_si256(it + 5, v256);
		case 5: _mm256_store_si256(it + 4, v256);
		case 4: _mm256_store_si256(it + 3, v256);
		case 3: _mm256_store_si256(it + 2, v256);
		case 2: _mm256_store_si256(it + 1, v256);
		case 1: _mm256_store_si256(it + 0, v256);
		case 0: break;
	}

	// final 32-unaligned
	usz misaligned_by = count & 7;
	if (misaligned_by) {
		constexpr usz words_per_op = sizeof(__m256i) / sizeof(u32);
		usz p = (usz)end - (words_per_op - misaligned_by) * sizeof(u32);
		_mm256_storeu_si256((__m256i*)p, v256);
	}
}

static
void memset32_rep_stosd(void* dst_, u32 v, usz size) {
	constexpr usz align = 64;
	constexpr usz align_lomask = align - 1;

	u32* it = dst_;

	usz misaligned_by = (usz)it & align_lomask;
	if (misaligned_by) {
		misaligned_by = (align - misaligned_by) / sizeof(v);
		for (u32* it = dst_, *end = it + misaligned_by; it < end; ++it)
			*it = v;
		size -= misaligned_by;
	}

	__asm__ volatile ("rep stosl"
		:
		: "a"(v), "c"(size / sizeof(u32)), "D"(it)
		: "memory", "cc");
}

FLATTEN
void* memset32(void* dst_, u32 v, usz count) {
	if (count < 32 / sizeof(v)) {
		for (u32* it = dst_, *end = it + count; it < end; ++it)
			*it = v;
		return dst_;
	}

#	ifdef HAS_AVX2
	if LIKELY (count < KB(32) / sizeof(v)) {
		memset32_avx2(dst_, v, count);
		return dst_;
	}
#	endif

	memset32_rep_stosd(dst_, v, count);
	return dst_;
}

#else

void* memset32(void* dst_, u32 v, usz count) {
	for (u32* it = dst_, *end = it + count; it < end; ++it)
		*it = v;
	return dst_;
}

#endif

