#include <lt/elf.h>
#include <lt/dwarf.h>
#include <lt/darr.h>
#include <lt/mem.h>

static
void custom_assert(b8 success, lstr_t str) {
	if (success)
		return;

	lt_ferrf("assertion '%S' failed\n", str);
}

#undef LT_ASSERT
#define LT_ASSERT(x) custom_assert((x), CLSTR(#x))

static
void consume_content(lt_elf64_t* e, u8** it, u32 (*fmts)[2], usz fmt_count, lstr_t* out_path) {
	for (usz i = 0; i < fmt_count; ++i) {
		switch (fmts[i][0]) {
		case LT_DWARF_LNCT_PATH:
			LT_ASSERT(fmts[i][1] == LT_DWARF_FORM_LINE_STRP);
			if (out_path) {
				u32 offs;
				memcpy(&offs, *it, sizeof(u32));
				*out_path = lt_elf64_str(e, e->dbgline_strtab_sh, offs);
			}
			*it += sizeof(u32);
			break;

		case LT_DWARF_LNCT_DIR_INDEX:
			LT_ASSERT(fmts[i][1] == LT_DWARF_FORM_UDATA);
			lt_dwarf_uleb128(it);
			break;

		case LT_DWARF_LNCT_TIMESTAMP:
			LT_ASSERT_NOT_REACHED();
			break;

		case LT_DWARF_LNCT_SIZE:
			LT_ASSERT_NOT_REACHED();
			break;

		case LT_DWARF_LNCT_MD5:
			LT_ASSERT(fmts[i][1] == LT_DWARF_FORM_DATA16);
			*it += 16;
			break;

		default:
			lt_ferrf("unhandled content type 0x%hd\n", fmts[i][0]);
		}
	}
}

#include <lt/io.h>

#define MAX_FILE_FMT_COUNT 8
#define MAX_DIR_FMT_COUNT 8

lt_err_t lt_elf64_lookup_vaddr(lt_elf64_t* e, usz vaddr, usz* out_line, lstr_t* out_file_name, void* p_) {
	if (!e->dbgline_sh || !e->dbgline_strtab_sh)
		return LT_ERR_UNKNOWN;

	lt_dwarf_stmt_prologue_t* p;
	if (!p_)
		p = lt_elf64_offs(e, e->dbgline_sh->offset);
	else
		p = p_;

	LT_ASSERT(p->max_ops_per_instr == 1);
	LT_ASSERT(p->addr_size == 8);

	// Read opcode lengths
	u8* standard_opcode_lengths = (u8*)p + sizeof(lt_dwarf_stmt_prologue_t);
	usz standard_opcode_length_count = p->opcode_base - 1;
	u8* it = standard_opcode_lengths + standard_opcode_length_count;

	// Read include directories
	u8 dir_fmt_count = *it++;
	u32 dir_fmts[MAX_DIR_FMT_COUNT][2];

	LT_ASSERT(dir_fmt_count < MAX_DIR_FMT_COUNT);

	for (usz i = 0; i < dir_fmt_count; ++i) {
		dir_fmts[i][0] = lt_dwarf_uleb128(&it); // content type
		dir_fmts[i][1] = lt_dwarf_uleb128(&it); // form
	}

	u32 dir_count = lt_dwarf_uleb128(&it);
	lt_darr(lstr_t) dirs = lt_darr_create(lstr_t, 32, lt_libc_heap);
	for (usz i = 0; i < dir_count; ++i) {
		lstr_t path = NLSTRI();
		consume_content(e, &it, dir_fmts, dir_fmt_count, &path);
		lt_darr_push(dirs, path);

	}

	// Read compilation units
	lt_darr(lstr_t) filenames = lt_darr_create(lstr_t, 32, lt_libc_heap);

	u8 file_fmt_count = *it++;
	u32 file_fmts[MAX_FILE_FMT_COUNT][2];

	LT_ASSERT(file_fmt_count < MAX_FILE_FMT_COUNT);

	for (usz i = 0; i < file_fmt_count; ++i) {
		file_fmts[i][0] = lt_dwarf_uleb128(&it); // content type
		file_fmts[i][1] = lt_dwarf_uleb128(&it); // form
	}

	u32 file_count = lt_dwarf_uleb128(&it);
	for (usz i = 0; i < file_count; ++i) {
		lstr_t path = NLSTRI();
		consume_content(e, &it, file_fmts, file_fmt_count, &path);
		lt_darr_push(filenames, path);
	}

	lt_dwarf_lns_t m;
	lt_dwarf_lns_init(&m, p);

	LT_ASSERT(it == (u8*)&p->min_instr_len + p->header_len);

	u32 last_line = 1;
	while (m.addr <= vaddr) {
		last_line = m.line;

		if (it >= (u8*)&p->version + p->unit_len) {
			lt_darr_destroy(dirs);
			lt_darr_destroy(filenames);

			if (it < (u8*)lt_elf64_offs(e, e->dbgline_sh->offset) + e->dbgline_sh->size)
				return lt_elf64_lookup_vaddr(e, vaddr, out_line, out_file_name, it);
			else
				return LT_ERR_NOT_FOUND;
		}

		it += lt_dwarf_execute(&m, p, it);
	}

	*out_file_name = filenames[m.file];
	*out_line = last_line;

	lt_darr_destroy(filenames);
	lt_darr_destroy(dirs);
	return LT_SUCCESS;
}

