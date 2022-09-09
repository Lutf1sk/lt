#ifndef LT_ASM_H
#define LT_ASM_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

// AMD64

typedef
struct lt_instr_stream {
	u8* it;
	u8* end;
	lt_io_callback_t callb;
	void* usr;
} lt_instr_stream_t;

lt_instr_stream_t lt_instr_stream_create(void* data, usz size, lt_io_callback_t callb, void* usr);
usz lt_x64_disasm_instr(lt_instr_stream_t* stream);

b8 lt_instr_stream_read(lt_instr_stream_t* stream, void* out, usz size);
b8 lt_instr_stream_consume(lt_instr_stream_t* stream, void* out, usz size);

#endif
