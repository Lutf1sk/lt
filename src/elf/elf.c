#include <lt/elf.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/debug.h>

b8 lt_elf_verify_magic(void* fh) {
	LT_ASSERT(fh);
	return memcmp(fh, (u8[]){ 0x7F,'E','L','F' }, 4) == 0;
}

lstr_t lt_elf64_str(const lt_elf64_t e[static 1], const lt_elf64_sh_t strtab_sh[static 1], usz offs) {
	LT_ASSERT(offs < strtab_sh->size);

	char* strtab = lt_elf64_offs(e, strtab_sh->offset);
	char* str = strtab + offs;
	char* end = strtab + strtab_sh->size;
	return LSTR(str, strnlen(str, end - str));
}

void lt_elf64_init(void* data, usz size, lt_elf64_t e[static 1]) {
	e->fh = data;
	e->base = data;
	e->size = size;
	e->sh_strtab_sh = lt_elf64_sh(e, e->fh->sh_strtab_index);

	e->symtab = NULL;
	e->sym_count = 0;
	e->sym_strtab_sh = NULL;

	e->dbgline_sh = NULL;
	e->dbgline_strtab_sh = NULL;

	for (usz i = 0; i < e->fh->sh_count; ++i) {
		lt_elf64_sh_t* sh = lt_elf64_sh(e, i);
		lstr_t name = lt_elf64_str(e, e->sh_strtab_sh, sh->name_stab_offs);

		switch (sh->type) {
		case LT_ELF_SH_STRTAB:
			if (lt_lseq(name, CLSTR(".strtab")))
				e->sym_strtab_sh = sh;
			break;

		case LT_ELF_SH_PROGBITS:
			if (lt_lseq(name, CLSTR(".debug_line")))
				e->dbgline_sh = sh;
			else if (lt_lseq(name, CLSTR(".debug_line_str")))
				e->dbgline_strtab_sh = sh;
			break;

		case LT_ELF_SH_SYMTAB:
			e->symtab = lt_elf64_offs(e, sh->offset);
			e->sym_count = sh->size / sizeof(lt_elf64_sym_t);
			break;
		}
	}
}

usz lt_elf64_vaddr_to_offs(const lt_elf64_t e[static 1], usz vaddr) {
	for (usz i = 0; i < e->fh->ph_count; ++i) {
		lt_elf64_ph_t* ph = lt_elf64_ph(e, i);
		if (ph->type != LT_ELF_PH_LOAD)
			continue;
		if (vaddr >= ph->vaddr && vaddr < ph->vaddr + ph->file_size)
			vaddr += ph->offset - ph->vaddr;
	}
	return vaddr;
}

lt_elf64_sh_t* lt_elf64_sh_by_name(const lt_elf64_t e[static 1], lstr_t name) {
	for (usz i = 0; i < e->fh->sh_count; ++i) {
		lt_elf64_sh_t* sh = lt_elf64_sh(e, i);
		lstr_t shname = lt_elf64_str(e, e->sh_strtab_sh, sh->name_stab_offs);
		if (lt_lseq(shname, name))
			return sh;
	}
	return NULL;
}

lt_elf64_sym_t* lt_elf64_sym_by_name(const lt_elf64_t e[static 1], lstr_t name) {
	if (!e->sym_strtab_sh)
		return NULL;

	for (isz i = 0; i < e->sym_count ; ++i) {
		lstr_t symname = lt_elf64_str(e, e->sym_strtab_sh, e->symtab[i].name_stab_offs);
		if (lt_lseq(symname, name))
			return &e->symtab[i];
	}
	return NULL;
}

lt_elf64_sym_t* lt_elf64_sym_by_vaddr(const lt_elf64_t e[static 1], usz vaddr) {
	if (!e->sym_strtab_sh)
		return NULL;

	for (usz i = 0; i < e->sym_count; ++i) {
		lt_elf64_sym_t* sym = &e->symtab[i];
		if (vaddr >= sym->value && vaddr < sym->value + sym->size)
			return sym;
	}
	return NULL;
}

