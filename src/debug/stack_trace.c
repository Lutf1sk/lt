#include <lt/debug.h>
#include <lt/elf.h>
#include <lt/io.h>
#include <lt/ansi.h>
#include <lt/mem.h>

#ifdef LT_LINUX

// Hack to detect the end of the call stack
#define MIN_FRAME (void*)0x100000

#include <lt/str.h>

void lt_print_instr_ptr(usz addr) {
	if (!lt_debug_executable)
		lt_printf("["LT_FG_BCYAN"0x%hz"LT_RESET"] in function '"LT_BOLD"<unknown>"LT_RESET"'\n", addr);

	lstr_t name = lt_debug_symname_at((void*)addr);
	usz elf_vaddr = addr - lt_debug_load_addr;

	lstr_t filename;
	usz linenum;
	if (lt_elf64_lookup_vaddr(lt_debug_executable, elf_vaddr, &linenum, &filename, NULL) == LT_SUCCESS)
		lt_printf("["LT_FG_BCYAN"0x%hz"LT_RESET"] in function '"LT_BOLD"%S"LT_RESET"' in "LT_BOLD"%S:%uz"LT_RESET" \n", addr, name, filename, linenum);
	else
		lt_printf("["LT_FG_BCYAN"0x%hz"LT_RESET"] in function '"LT_BOLD"%S"LT_RESET"'\n", addr, name);
}

void lt_stack_trace(usz skip_frames) {
	void* frame = LT_FRAME_ADDR;

	while (frame > MIN_FRAME && skip_frames--)
		frame = LT_FRAME_PREV_ADDR(frame);

	while (frame > MIN_FRAME) {
		usz ret_addr = (usz)LT_FRAME_RETURN_ADDR(frame) - 1; // Subtract one, since a call instruction always pushes the adress of the NEXT instruction
		lt_printf("\t");
		lt_print_instr_ptr(ret_addr);
		frame = LT_FRAME_PREV_ADDR(frame);
	}
	return;
}

#elif defined(LT_WINDOWS)

void lt_stack_trace(usz skip_frames) {
	lt_printf("\tstack trace unavailable for win32 build\n");
}

#endif
