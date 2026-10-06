#include <lt2/common.h>
#include <lt2/bits.h>

#ifdef ON_UNIX
#	include <lt2/posix.h>

#	ifndef __USE_GNU
#		define __USE_GNU
#	endif
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE
#	endif
#	include <sys/mman.h>

ringbuf_t vmap_ringbuf(usz size, err* err) {
	if UNLIKELY (!size || size > (u64)INT64_MAX + 1) {
		throw(err, ERR_BAD_ARGUMENT, "invalid ring buffer size");
		goto err0;
	}

	size = align(next_pow2(size), VM_PAGE_SIZE);

	void* lo = mmap(NULL, size * 2, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if UNLIKELY (lo == MAP_FAILED) {
		throw_errno(err);
		goto err0;
	}

	void* hi = mremap(lo, 0, size,  MREMAP_MAYMOVE | MREMAP_FIXED, (u8*)lo + size);
	if UNLIKELY (hi == MAP_FAILED) {
		throw_errno(err);
		goto err1;
	}

	return (ringbuf_t) {
		.first = lo,
		.base  = lo,
		.end   = hi,
		.size  = size,
		.mask  = size - 1,
	};

err1:
	munmap(lo, size * 2);
err0:
	return (ringbuf_t) {0};
}

#endif // ON_UNIX

usz rb_write(ringbuf_t* rb, const void* data, usz size) {
	usz avail = rb->size - rb->used;
	if (size > avail)
		size = avail;

	memcpy(rb->first + rb->used, data, size);
	rb->used += size;
	return size;
}

usz rb_read(ringbuf_t* rb, void* data, usz size) {
	if (size > rb->used)
		size = rb->used;

	memcpy(data, rb->first, size);
	rb->first = rb->base + ((rb->first - rb->base + size) & rb->mask);
	rb->used -= size;
	return size;
}

usz rb_skip(ringbuf_t* rb, usz size) {
	if (size > rb->used)
		size = rb->used;
	rb->first = rb->base + ((rb->first - rb->base + size) & rb->mask);
	rb->used -= size;
	return size;
}

