#include <lt/net.h>
#include <lt/mem.h>
#include <lt/math.h>

lt_err_t lt_sockstream_create(lt_sockstream_t stream[static 1], lt_socket_t sock[static 1], usz buffer_size, lt_alloc_t alloc[static 1]) {
	LT_ASSERT(buffer_size);

	stream->socket = sock;
	if (!(stream->buffer = lt_malloc(alloc, buffer_size)))
		return LT_ERR_OUT_OF_MEMORY;
	stream->buffer_size = buffer_size;
	stream->it = stream->buffer;
	stream->bytes_avail = 0;

	return LT_SUCCESS;
}

void lt_sockstream_destroy(const lt_sockstream_t stream[static 1], lt_alloc_t alloc[static 1]) {
	lt_mfree(alloc, stream->buffer);
}

isz lt_sockstream_read(lt_sockstream_t stream[static 1], void* data, usz size) {
	if (!stream->bytes_avail) {
		stream->it = stream->buffer;
		isz res = lt_socket_recv(stream->socket, stream->buffer, stream->buffer_size);
		if (res <= 0)
			return res;
		stream->bytes_avail = res;
	}

	size = lt_min(stream->bytes_avail, size);
	memcpy(data, stream->it, size);
	stream->it += size;
	stream->bytes_avail -= size;
	return size;
}

isz lt_sockstream_read_fixed(lt_sockstream_t stream[static 1], void* data, usz size) {
	u8 *it = data, *end = it + size;
	while (it < end) {
		isz res = lt_sockstream_read(stream, it, end - it);
		if (res < 0)
			return res;
		it += res;
	}
	return size;
}
