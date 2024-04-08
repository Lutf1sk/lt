#include <lt/asm.h>
#include <lt/mem.h>

lt_instr_stream_t lt_instr_stream_create(lt_write_fn_t callb, void* usr, const void* data, usz size) {
	lt_instr_stream_t stream;
	stream.it = data;
	stream.end = (u8*)data + size;
	stream.callb = callb;
	stream.usr = usr;
	return stream;
}

b8 lt_instr_stream_read(lt_instr_stream_t stream[static 1], void* out, usz size) {
	if (stream->it + size > stream->end)
		return 0;
	memcpy(out, stream->it, size);
	return 1;
}

b8 lt_instr_stream_consume(lt_instr_stream_t stream[static 1], void* out, usz size) {
	if (stream->it + size > stream->end)
		return 0;
	memcpy(out, stream->it, size);
	stream->it += size;
	return 1;
}

