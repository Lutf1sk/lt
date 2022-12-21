#include <lt/net.h>
#include <lt/mem.h>
#include <lt/math.h>

lt_err_t lt_sockstream_create(lt_sockstream_t* stream, lt_socket_t* sock, usz buffer_size, lt_alloc_t* alloc) {
	LT_ASSERT(buffer_size);

	stream->socket = sock;
	if (!(stream->buffer = lt_malloc(alloc, buffer_size)))
		return LT_ERR_OUT_OF_MEMORY;
	stream->buffer_size = buffer_size;
	stream->it = stream->buffer;
	stream->bytes_avail = 0;

	return LT_SUCCESS;
}

void lt_sockstream_destroy(lt_sockstream_t* stream, lt_alloc_t* alloc) {
	lt_mfree(alloc, stream->buffer);
}

isz lt_sockstream_read(lt_sockstream_t* stream, void* data, usz size) {
	if (!stream->bytes_avail) {
		stream->it = stream->buffer;
		isz res = lt_socket_recv(stream->socket, stream->buffer, stream->buffer_size);
		if (res <= 0)
			return res;
		stream->bytes_avail = res;
	}

	size = lt_min_usz(stream->bytes_avail, size);
	memcpy(data, stream->it, size);
	stream->it += size;
	stream->bytes_avail -= size;
	return size;
}

lt_err_t lt_sockstream_read_fixed(lt_sockstream_t* stream, void* data, usz size) {
	u8 *it = data, *end = it + size;
	while (it < end) {
		isz res = lt_sockstream_read(stream, it, end - it);
		if (res < 0)
			return res;
		it += res;
	}
	return LT_SUCCESS;
}
