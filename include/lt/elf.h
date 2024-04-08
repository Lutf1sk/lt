#ifndef LT_ELF_H
#define LT_ELF_H 1

#include <lt/lt.h>
#include <lt/err.h>

#define LT_ELF_VERSION_CURRENT 1

typedef
enum lt_elf_class {
	LT_ELFCLASS_32 = 1,
	LT_ELFCLASS_64 = 2,
} lt_elf_class_t;

typedef
enum lt_elf_osabi {
	LT_ELFABI_SYSV = 0,
} lt_elf_osabi_t;

typedef
enum lt_elf_encoding {
	LT_ELFENC_LSB = 1,
	LT_ELFENC_MSB = 2,
} lt_elf_encoding_t;

typedef
enum lt_elf_objtype {
	LT_ELFTYPE_REL = 1,
	LT_ELFTYPE_EXEC = 2,
	LT_ELFTYPE_DYN = 3,
	LT_ELFTYPE_CORE = 4,
} lt_elf_objtype_t;

typedef
enum lt_elf_arch {
	LT_ELFARCH_I386 = 3,
	LT_ELFARCH_AMD64 = 62,
} lt_elf_arch_t;

typedef
LT_PACKED_STRUCT(lt_elf64_fh) {
	u8 magic[4];

	u8 class;
	u8 encoding;
	u8 header_version;
	u8 osabi;

	u8 pad[8];

	u16 obj_type;
	u16 arch;
	u32 version;

	u64 entry;
	u64 ph_offset;
	u64 sh_offset;

	u32 cpu_flags;

	u16 fh_size;

	u16 ph_size;
	u16 ph_count;
	u16 sh_size;
	u16 sh_count;

	u16 sh_strtab_index;
} lt_elf64_fh_t;

typedef
LT_PACKED_STRUCT(lt_elf32_fh) {
	u8 magic[4];

	u8 class;
	u8 encoding;
	u8 header_version;
	u8 osabi;

	u8 pad[8];

	u16 obj_type;
	u16 arch;
	u32 version;

	u32 entry;
	u32 ph_offset;
	u32 sh_offset;

	u32 cpu_flags;

	u16 fh_size;

	u16 ph_size;
	u16 ph_count;
	u16 sh_size;
	u16 sh_count;

	u16 sh_strtab_index;
} lt_elf32_fh_t;

typedef
enum lt_elf_ph_type {
	LT_ELF_PH_NULL		= 0,	// Unused segment
	LT_ELF_PH_LOAD		= 1,	// Loadable segment
	LT_ELF_PH_DYNAMIC	= 2,	// Dynamic linking info
	LT_ELF_PH_INTERP	= 3,	// Interpreter name
	LT_ELF_PH_NOTE		= 4,	// Auxiliary info
	LT_ELF_PH_SHLIB		= 5,	// Reserved
	LT_ELF_PH_PHDR		= 6,	// Entry for header table
	LT_ELF_PH_TLS		= 7,	// Thread-local storage segment
} lt_elf_ph_type_t;

typedef
enum lt_elf_ph_flags {
	LT_ELF_PH_X = 1,
	LT_ELF_PH_W = 2,
	LT_ELF_PH_R = 4,
} lt_elf_ph_flags_t;

typedef
LT_PACKED_STRUCT(lt_elf64_ph) {
	u32 type;
	u32 flags;
	u64 offset;

	u64 vaddr;
	u64 paddr;
	u64 file_size;
	u64 mem_size;
	u64 alignment;
} lt_elf64_ph_t;

typedef
LT_PACKED_STRUCT(lt_elf32_ph) {
	u32 type;
	u32 offset;

	u32 vaddr;
	u32 paddr;
	u32 file_size;
	u32 mem_size;
	u32 flags;
	u32 alignment;
} lt_elf32_ph_t;

typedef
enum lt_elf_sh_type {
	LT_ELF_SH_NULL			= 0,	// Unused section
	LT_ELF_SH_PROGBITS		= 1,	// Program data
	LT_ELF_SH_SYMTAB		= 2,	// Symbol table
	LT_ELF_SH_STRTAB		= 3,	// String table
	LT_ELF_SH_RELA			= 4,	// 'RelAdd' table
	LT_ELF_SH_HASH			= 5,	// Symbol hash table
	LT_ELF_SH_DYNAMIC		= 6,	// Dynamic linking info
	LT_ELF_SH_NOTE			= 7,	// Notes
	LT_ELF_SH_NOBITS		= 8,	// Zero-initialized program data
	LT_ELF_SH_REL			= 9,	// 'Rel' table
	LT_ELF_SH_SHLIB			= 10,	// Reserved
	LT_ELF_SH_DYNSYM		= 11,	// Dynamic linker symbol table
	LT_ELF_SH_INIT_ARRAY	= 14,	// Constructor array
	LT_ELF_SH_FINIT_ARRAY	= 15,	// Destructor array
	LT_ELF_SH_PREINIT_ARRAY	= 16,	// Pre-constructor array
	LT_ELF_SH_GROUP			= 17,	// Section group
	LT_ELF_SH_SYMTAB_SHNDX	= 18,	// Extended section indices
} lt_elf_sh_type_t;

// Special section indices
#define LT_ELFSH_UNDEF	0		// Undefined section
#define LT_ELFSH_ABS	0xFFF1	// Symbol is absolute
#define LT_ELFSH_COMMON	0xFFF2	// Symbol labels an unallocated common block
#define LT_ELFSH_XINDEX	0xFFFF	// Index is extra table

typedef
LT_PACKED_STRUCT(lt_elf64_sh) {
	u32 name_stab_offs;

	u32 type;
	u64 flags;

	u64 addr;
	u64 offset;
	u64 size;

	u32 link;
	u32 info;

	u64 addr_align;
	u64 ent_size;
} lt_elf64_sh_t;

typedef
LT_PACKED_STRUCT(lt_elf32_sh) {
	u32 name_stab_offs;

	u32 type;
	u32 flags;

	u32 addr;
	u32 offset;
	u32 size;

	u32 link;
	u32 info;

	u32 addr_align;
	u32 ent_size;
} lt_elf32_sh_t;

typedef
enum lt_elf_sym_type {
	// Binding
	LT_ELF_SYM_LOCAL	= 0x00,
	LT_ELF_SYM_GLOBAL	= 0x10,
	LT_ELF_SYM_WEAK		= 0x20,

	// Type
	LT_ELF_SYM_NOTYPE	= 0x00,
	LT_ELF_SYM_OBJECT	= 0x01,	// Data object
	LT_ELF_SYM_FUNC		= 0x02,	// Function or executable code
	LT_ELF_SYM_SECTION	= 0x03,	// Section
	LT_ELF_SYM_FILE		= 0x04,	// Symbol name is the name of a source file
	LT_ELF_SYM_COMMON	= 0x05,	// Uninitialized common block
	LT_ELF_SYM_TLS		= 0x06,	// Thread-Local-Storage

	LT_ELF_SYM_BIND_MASK	= 0xF0,
	LT_ELF_SYM_TYPE_MASK	= 0x0F,

	// Visibility
	LT_ELF_SYM_DEFAULT		= 0x00, // Use binding type
	LT_ELF_SYM_INTERNAL		= 0x01, // Local
	LT_ELF_SYM_HIDDEN		= 0x02, // Local
	LT_ELF_SYM_PROTECTED	= 0x03, // Prioritize over all external definitions
} lt_elf_sym_type_t;


typedef
LT_PACKED_STRUCT(lt_elf64_sym) {
	u32 name_stab_offs;
	u8 info;
	u8 other;
	u16 section;
	u64 value;
	u64 size;
} lt_elf64_sym_t;

typedef
LT_PACKED_STRUCT(lt_elf32_sym) {
	u32 name_stab_offs;
	u32 value;
	u32 size;
	u8 info;
	u8 other;
	u16 section;
} lt_elf32_sym_t;

b8 lt_elf_verify_magic(void* fh);

typedef
enum lt_elf_dyn_type {
	LT_ELF_DYN_NULL			= 0,	// Marks end of dynamic section
	LT_ELF_DYN_NEEDED		= 1,	// Name of needed shared object
	LT_ELF_DYN_PLTRELSZ		= 2,	// Size of PLT relocs
	LT_ELF_DYN_PLTGOT		= 3,	// CPU defined value
	LT_ELF_DYN_HASH			= 4,	// Offset of symbol hash table
	LT_ELF_DYN_STRTAB		= 5,	// Offset of string table
	LT_ELF_DYN_SYMTAB		= 6,	// Offset of symbol table
	LT_ELF_DYN_RELA			= 7,	// Offset of 'RelAdd' relocs
	LT_ELF_DYN_RELASZ		= 8,	// Total size of 'RelAdd' relocs
	LT_ELF_DYN_RELAENT		= 9,	// Size of one 'RelAdd' reloc
	LT_ELF_DYN_STRSZ		= 10,	// Size of string table
	LT_ELF_DYN_SYMENT		= 11,	// Size of one symbol table entry
	LT_ELF_DYN_INIT			= 12,	// Offset of init function
	LT_ELF_DYN_FINI			= 13,	// Offset of termination function
	LT_ELF_DYN_SONAME		= 14,	// Name of shared object
	LT_ELF_DYN_RPATH		= 15,	// Library search path (deprecated)
	LT_ELF_DYN_SYMBOLIC		= 16,	// Symbol search offset
	LT_ELF_DYN_REL			= 17,	// Address of 'Rel' relocs
	LT_ELF_DYN_RELSZ		= 18,	// Total size of 'Rel' relocs
	LT_ELF_DYN_RELENT		= 19,	// Size of one 'Rel' reloc
	LT_ELF_DYN_PLTREL		= 20,	// PLT reloc type
	LT_ELF_DYN_DEBUG		= 21,	// Unspecified
	LT_ELF_DYN_TEXTREL		= 22,	// Reloc can modify .text
	LT_ELF_DYN_JMPREL		= 23,	// Offset of PLT relocs
	LT_ELF_DYN_BIND_NOW		= 24,	// Process relocations of object
	LT_ELF_DYN_INIT_ARRAY	= 25,	// Array of init function offsets
	LT_ELF_DYN_FINI_ARRAY	= 26,	// Array of termination function offsets
	LT_ELF_DYN_INIT_ARRAYSZ	= 27,	// Size of ELFDYN_INIT_ARR
	LT_ELF_DYN_FINI_ARRAYSZ	= 28,	// Size of ELFDYN_FINI_ARR
	LT_ELF_DYN_RUNPATH		= 29,	// Library search path
	LT_ELF_DYN_FLAGS		= 30,	// Flags for loaded object
} lt_elf_dyn_type_t;

typedef
LT_PACKED_STRUCT(lt_elf64_dyn) {
	i64 type;
	u64 val;
} lt_elf64_dyn_t;


typedef
struct lt_elf64 {
	lt_elf64_fh_t* fh;
	void* base;
	usz size;

	lt_elf64_sh_t* sh_strtab_sh;
	lt_elf64_sh_t* sym_strtab_sh;

	lt_elf64_sym_t* symtab;
	usz sym_count;

	lt_elf64_sh_t* dbgline_sh;
	lt_elf64_sh_t* dbgline_strtab_sh;
} lt_elf64_t;

static LT_INLINE
lt_elf64_sh_t* lt_elf64_sh(const lt_elf64_t e[static 1], usz i) {
	return (void*)((usz)e->fh + e->fh->sh_offset + e->fh->sh_size * i);
}

static LT_INLINE
lt_elf64_ph_t* lt_elf64_ph(const lt_elf64_t e[static 1], usz i) {
	return (void*)((usz)e->fh + e->fh->ph_offset + e->fh->ph_size * i);
}

static LT_INLINE
void* lt_elf64_offs(const lt_elf64_t e[static 1], usz offs) {
	return (void*)((usz)e->base + offs);
}

lstr_t lt_elf64_str(const lt_elf64_t e[static 1], const lt_elf64_sh_t strtab_sh[static 1], usz offs);

void lt_elf64_init(void* data, usz size, lt_elf64_t out_e[static 1]);

usz lt_elf64_vaddr_to_offs(const lt_elf64_t e[static 1], usz vaddr);

lt_elf64_sh_t* lt_elf64_sh_by_name(const lt_elf64_t e[static 1], lstr_t name);

lt_elf64_sym_t* lt_elf64_sym_by_name(const lt_elf64_t e[static 1], lstr_t name);
lt_elf64_sym_t* lt_elf64_sym_by_vaddr(const lt_elf64_t e[static 1], usz vaddr);

lt_err_t lt_elf64_lookup_vaddr(const lt_elf64_t e[static 1], usz vaddr, usz out_line[static 1], lstr_t out_file_name[static 1], void* addr);

#endif
