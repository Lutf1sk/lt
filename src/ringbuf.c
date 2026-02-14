#include <lt2/common.h>
#include <lt2/bits.h>

#ifdef ON_UNIX
#	include <lt2/posix.h>

#	define __USE_GNU
#	include <sys/mman.h>

ringbuf_t vmap_ringbuf(usz size, err* err) {
	size = align(size, VM_PAGE_SIZE);

	void* lo = mmap(NULL, size * 2, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (lo == MAP_FAILED) {
		throw_errno(err);
		goto err0;
	}

	void* hi = mremap(lo, 0, size,  MREMAP_MAYMOVE | MREMAP_FIXED, (u8*)lo + size);
	if (hi == MAP_FAILED) {
		throw_errno(err);
		goto err1;
	}

	return (ringbuf_t) {
		.first = lo,
		.base  = lo,
		.end   = hi,
		.size  = size,
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
	rb->first = rb->base + (rb->first - rb->base + size) % rb->size;
	rb->used -= size;
	return size;
}

