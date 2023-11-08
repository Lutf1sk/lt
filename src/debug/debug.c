#include <lt/debug.h>
#include <lt/io.h>
#include <lt/mem.h>
#include <lt/elf.h>
#include <lt/ansi.h>

#define __USE_GNU
#include <signal.h>
#include <ucontext.h>
#include <stdlib.h>

static
lt_elf64_t debug_executable;

lt_elf64_t* lt_debug_executable = NULL;
usz lt_debug_load_addr;

static
void handle_sigsegv(int sig, siginfo_t* si, void* usr) {
	ucontext_t* c = usr;
	void* instr_addr = (void*)c->uc_mcontext.gregs[REG_RIP];

	lt_printf(LT_FG_BRED"error"LT_RESET": segmentation fault accessing "LT_FG_BCYAN"0x%hz"LT_RESET"\n\t", (usz)si->si_addr);
	lt_print_instr_ptr((usz)instr_addr);
	lt_stack_trace(2);
	exit(sig);
}

void lt_debug_init(void) {
	// Load debug information from the running executable
	lstr_t exec_data;
	if (lt_freadallp(CLSTR("/proc/self/exe"), &exec_data, lt_libc_heap) != LT_SUCCESS) {
		lt_werrf("failed to read from /proc/self/exe\n");
		return;
	}

	lt_elf64_init(exec_data.str, exec_data.len, &debug_executable);

	lt_elf64_sym_t* sym = lt_elf64_sym_by_name(&debug_executable, CLSTR("lt_stack_trace"));
	if (!sym) {
		lt_werrf("could not find anchor symbol in debug executable\n");
		lt_mfree(lt_libc_heap, exec_data.str);
		return;
	}

	lt_debug_executable = &debug_executable;
	lt_debug_load_addr = (usz)lt_stack_trace - sym->value;

	// Register signal handlers
	struct sigaction sact = {};
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = SA_SIGINFO;
	sact.sa_sigaction = handle_sigsegv;
	if (sigaction(SIGSEGV, &sact, NULL) < 0)
		lt_werrf("failed to register SIGSEGV handler\n");
}

lstr_t lt_debug_symname_at(void* ptr) {
	lstr_t name = CLSTR("<unknown>");
	if (!lt_debug_executable)
		return name;

	usz elf_vaddr = (usz)ptr - lt_debug_load_addr;
	lt_elf64_sym_t* sym = lt_elf64_sym_by_vaddr(lt_debug_executable, elf_vaddr);
	if (sym && lt_debug_executable->sym_strtab_sh)
		name = lt_elf64_str(lt_debug_executable, lt_debug_executable->sym_strtab_sh, sym->name_stab_offs);
	return name;
}

