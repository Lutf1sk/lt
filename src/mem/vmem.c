#include <lt/mem.h>
#include <lt/lt.h>
#include <lt/debug.h>

#ifdef LT_UNIX
#	include <sys/mman.h>

static u32 prot_conv_tab[LT_VMEM_ACCESS_MASK + 1] = {
	[0]           = PROT_NONE,
	[LT_VMEM_R]   = PROT_READ,
	[LT_VMEM_W]   = PROT_WRITE,
	[LT_VMEM_RW]  = PROT_READ  | PROT_WRITE,
	[LT_VMEM_X]   = PROT_EXEC,
	[LT_VMEM_RX]  = PROT_READ  | PROT_EXEC,
	[LT_VMEM_WX]  = PROT_WRITE | PROT_EXEC,
	[LT_VMEM_RWX] = PROT_READ  | PROT_WRITE | PROT_EXEC,
};

#ifndef MAP_UNINITIALIZED
#	define MAP_UNINITIALIZED 0
#endif

#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	include <memoryapi.h>

static u32 prot_conv_tab[LT_VMEM_ACCESS_MASK + 1] = {
	[0]           = PAGE_NOACCESS,
	[LT_VMEM_R]   = PAGE_READONLY,
	[LT_VMEM_RW]  = PAGE_READWRITE,
	[LT_VMEM_X]   = PAGE_EXECUTE,
	[LT_VMEM_RX]  = PAGE_EXECUTE_READ,
	[LT_VMEM_RWX] = PAGE_EXECUTE_READWRITE,
};

#endif

void* lt_vmmap(void* addr, usz page_count, u32 flags) {
	int prot = prot_conv_tab[flags & LT_VMEM_ACCESS_MASK];
	void* mem;

#ifdef LT_UNIX
	int real_flags = MAP_PRIVATE | MAP_ANONYMOUS;
	if (flags & LT_VMEM_COMMIT)   real_flags |= MAP_POPULATE;
	if (flags & LT_VMEM_NOSWAP)   real_flags |= MAP_NORESERVE;
	if (flags & LT_VMEM_NONBLOCK) real_flags |= MAP_NONBLOCK;
	if (!(flags & LT_VMEM_ZERO))  real_flags |= MAP_UNINITIALIZED;

	if (addr) real_flags |= MAP_FIXED;
	mem = mmap(addr, page_count << LT_PAGE_SHIFT, prot, real_flags, -1, 0);
	if LT_LIKELY (mem != MAP_FAILED) {
		return mem;
	}

#elif defined(LT_WINDOWS)
	if LT_UNLIKELY (prot == 0) {
		lt_werrf("protection flags are not supported for this platform\n");
		prot = PAGE_EXECUTE_READWRITE,
	}

	int real_flags = MEM_RESERVE;
	if (flags & LT_VMEM_NOCACHE) prot       |= PAGE_NOCACHE
	if (flags & LT_VMEM_COMMIT)  real_flags |= MEM_COMMIT;
	mem = VirtualAlloc(addr, page_count << LT_PAGE_SHIFT, real_flags, prot);
	if LT_LIKELY (mem) {
		return mem;
	}
#endif

	lt_werrf("virtual page mapping failed\n");
	return NULL;
}

void lt_vmunmap(void* addr, usz page_count) {
#ifdef LT_UNIX
	munmap(addr, page_count << LT_PAGE_SHIFT);
#elif defined(LT_WINDOWS)
	VirtualFree(addr, 0, MEM_RELEASE);
#endif
}

void* lt_vmalloc_guarded(usz size, u32 flags) {
	usz pages = lt_size_to_page_count(size);
	void* mem = lt_vmmap(NULL, pages + 1, flags);
	if LT_UNLIKELY (!mem) {
		return NULL;
	}

	if LT_UNLIKELY (!lt_vmmap((u8*)mem + (pages << LT_PAGE_SHIFT), 1, 0)) {
		lt_werrf("failed to map guard page\n");
		lt_vmunmap(mem, pages + 1);
		return NULL;
	}
	return mem;
}

LT_FLATTEN
void lt_vmfree_guarded(void* addr, usz size) {
	lt_vmfree(addr, size + LT_PAGE_SIZE);
}

void* lt_vmalloc(usz size) {
	return lt_vmmap(NULL, lt_size_to_page_count(size), LT_VMEM_RW);
}

LT_FLATTEN
void lt_vmfree(void* addr, usz size) {
	lt_vmunmap(addr, lt_size_to_page_count(size));
}

