#include <lt2/bits.h>

#ifdef ON_UNIX
#	include <lt2/posix.h>

#	include <sys/mman.h>

b8 vmap(vmap_t* mappings, usz count, u32 flags, err* err) {
	int posix_flags = MAP_PRIVATE | MAP_ANONYMOUS; // MAP_NORESERVE ?

	usz page_size = VM_PAGE_SIZE;
	if (flags & VM_HUGE_PAGES) {
		page_size = VM_PAGE_SIZE_HUGE;
		posix_flags |= MAP_HUGETLB;
	}

	usz total_size = 0;
	vmap_t* end = mappings + count;
	for (vmap_t* vm = mappings; vm < end; ++vm) {
		vm->size       = align(vm->size,       page_size);
		vm->guard_size = align(vm->guard_size, page_size);
		total_size += vm->size + vm->guard_size; // can overflow
	}

	if (!total_size)
		return 0; // !! should zero the outputs

	void* block = mmap(NULL, total_size, 0, posix_flags, -1, 0);
	if (block == MAP_FAILED) {
		throw_errno(err);
		return 1;
	}

	u8* it = block;
	for (vmap_t* vm = mappings; vm < end; ++vm) {
		// !! if all mappings have the same permissions, this should really not need mprotect, it should be done in the mmap call
		if (mprotect(it, vm->size, posix_prot_tab[vm->permit & 3]) < 0) {
			throw_errno(err);
			munmap(block, total_size);
			return 1;
		}

		if (vm->out)
			*vm->out = it;
		if (vm->out_size)
			*vm->out_size = vm->size;

		vm->base       = it;
		vm->guard_base = it += vm->size;
		vm->guard_end  = it += vm->guard_size;
	}
	return 0;
}

void vunmap(vmap_t* mappings, usz count, err* err) {
	usz total_size = 0;
	for (vmap_t* it = mappings, *end = it + count; it < end; ++it)
		total_size += it->size + it->guard_size;
	if (!total_size)
		return;
	if (munmap(mappings[0].base, total_size) < 0)
		throw_errno(err);
}

#endif // ON_UNIX

