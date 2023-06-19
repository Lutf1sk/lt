#ifndef LT_DEBUG_H
#define LT_DEBUG_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

#define LT_FRAME_ADDR __builtin_frame_address(0)
#define LT_FRAME_PREV_ADDR(frame) (*(void**)(frame))
#define LT_FRAME_RETURN_ADDR(frame) (*((void**)(frame) + 1))
#define LT_RETURN_ADDR LT_FRAME_RETURN_ADDR(LT_FRAME_ADDR)

void lt_print_instr_ptr(usz addr);
void lt_stack_trace(usz skip_frames);

#define LT_DEBUG

typedef
struct lt_debug_caller {
	lstr_t lt_filename;
	lstr_t lt_function;
	usz lt_linenum;
} lt_debug_caller_t;

extern lt_elf64_t* lt_debug_executable;
extern usz lt_debug_load_addr;

void lt_debug_init(void);

lstr_t lt_debug_symname_at(void* ptr);

#define LT_DEBUG_CALLER ((lt_debug_caller_t[1]){ { .lt_filename = LT_FILENAME, .lt_function = LT_FUNCTION, .lt_linenum = LT_LINENUM } })

#ifdef LT_DEBUG
int lt_assert_failed(lstr_t file, usz line, lstr_t assertion);
int lt_assert_unreachable_failed(lstr_t file, usz line);
void lt_breakpoint(void);
#	define LT_DEBUG_INIT() lt_debug_init()

#	define LT_ASSERT(x) ((void)( (x) ? 0 : lt_assert_failed(LT_FILENAME, LT_LINENUM, CLSTR(#x)) ))
#	define LT_ASSERT_NOT_REACHED() lt_assert_unreachable_failed(LT_FILENAME, LT_LINENUM)
#	define LT_BREAKPOINT() lt_breakpoint()
#else
#	define LT_DEBUG_INIT() ((void)0)

#	define LT_ASSERT(x) ((void)0)
#	define LT_ASSERT_NOT_REACHED() ((void)0)
#	define LT_BREAKPOINT() ((void)0)
#endif


#endif
