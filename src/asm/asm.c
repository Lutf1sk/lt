#include <lt/asm.h>
#include <lt/mem.h>

lt_instr_stream_t lt_instr_stream_create(void* data, usz size, lt_io_callback_t callb, void* usr) {
	lt_instr_stream_t stream;
	stream.it = data;
	stream.end = (u8*)data + size;
	stream.callb = callb;
	stream.usr = usr;
	return stream;
}

b8 lt_instr_stream_read(lt_instr_stream_t* stream, void* out, usz size) {
	if (stream->it + size > stream->end)
		return 0;
	memcpy(out, stream->it, size);
	return 1;
}

b8 lt_instr_stream_consume(lt_instr_stream_t* stream, void* out, usz size) {
	if (stream->it + size > stream->end)
		return 0;
	memcpy(out, stream->it, size);
	stream->it += size;
	return 1;
}

