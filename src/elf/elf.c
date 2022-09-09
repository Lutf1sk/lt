#include <lt/elf.h>
#include <lt/mem.h>

b8 lt_elf_verify_magic(void* fh) {
	return memcmp(fh, (u8[]){ 0x7F,'E','L','F' }, 4) == 0;
}



