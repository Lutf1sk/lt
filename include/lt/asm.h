#ifndef LT_ASM_H
#define LT_ASM_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

// AMD64

typedef
struct lt_instr_stream {
	const u8* it;
	const u8* end;
	lt_write_fn_t callb;
	void* usr;
} lt_instr_stream_t;

lt_instr_stream_t lt_instr_stream_create(lt_write_fn_t callb, void* usr, const void* data, usz size);
usz lt_x64_disasm_instr(lt_instr_stream_t stream[static 1]);

b8 lt_instr_stream_read(lt_instr_stream_t stream[static 1], void* out, usz size);
b8 lt_instr_stream_consume(lt_instr_stream_t stream[static 1], void* out, usz size);

#endif
