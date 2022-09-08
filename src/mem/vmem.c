#include <lt/mem.h>
#include <lt/lt.h>

#ifdef LT_UNIX
#	include <sys/mman.h>

#	define MAP_PAGES(size) mmap(NULL, (size), PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
#	define UNMAP_PAGES(addr, size) munmap((addr), (size))
#	define MAP_PAGES_ERROR MAP_FAILED
#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	include <memoryapi.h>

#	define MAP_PAGES(size) VirtualAlloc(NULL, (size), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)
#	define UNMAP_PAGES(addr, size) VirtualFree((addr), 0, MEM_RELEASE)
#	define MAP_PAGES_ERROR NULL
#endif

void* lt_vmalloc(usz size) {
	void* mem = MAP_PAGES(size);
	if (mem == MAP_PAGES_ERROR)
		return NULL;
	return mem;
}

void lt_vmfree(void* addr, usz size) {
	UNMAP_PAGES(addr, size);
}

