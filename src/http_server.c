#include <lt2/http_server.h>
#include <lt2/time.h>
#include <lt2/str.h>
#include <lt2/log.h>

#include <ctype.h>
#include <signal.h>

usz send_raw(client_state* state, void* data, usz size, err* error) {
#ifdef LT_OPENSSL
	if (state->is_https)
		return socket_send_tls(state->tls, data, size, error);
	else
#endif
		return socket_send(state->socket, data, size, error);
}

static
void send_response_async($async, client_state* state, u64 timeout_in_us, err* err) {
	$enter_task();

	state->timeout_at_us = time_us() + timeout_in_us;
	while (state->response_it < state->response_end) {
		usz res;
#ifdef LT_OPENSSL
		if (state->is_https)
			res = socket_send_tls(state->tls, state->response_it, state->response_end - state->response_it, err);
		else
#endif
			res = socket_send(state->socket, state->response_it, state->response_end - state->response_it, err);
		state->response_it += res;
		if (res)
			state->timeout_at_us = time_us() + timeout_in_us;
		else if (time_us() >= state->timeout_at_us) {
			throw(err, ERR_TIMED_OUT, "response timed out");
			return;
		}
	}
}

void send_page($async, server_info* server, client_state* state, void (*page)($template)) {
	$enter_task();

	html_template template = {
		.it  = state->page_buf,
		.end = state->page_buf + sizeof(state->page_buf)
	};
	page(&template, state);
	state->response_it  = state->response = state->page_buf;
	state->response_end = state->response + (template.it - state->page_buf);

	$await($subtask, server->write_default_headers($subtask, server, state, state->response_end - state->response, ls("text/html")));
	$await($subtask, send_response_async($subtask, state, S_TO_US(5), err_warn));
}

void send_file($async, server_info* server, client_state* state, ls real_path) {
	$enter_task();

	ls file = fmapall(real_path, R, err_warn);
	if (!file.size) {
		$await($subtask, server->on_unmapped_request($subtask, server, state));
		return;
	}
	state->response_it  = state->response = file.ptr;
	state->response_end = file.ptr + file.size;

	$await($subtask, server->write_default_headers($subtask, server, state, state->response_end - state->response, mime_type_from_ext(real_path)));
	$await($subtask, send_response_async($subtask, state, S_TO_US(5), err_warn));

	funmap(lsrange(state->response, state->response_end), err_warn);
}

ls normalize_path(ls path) {
	u8* start = path.ptr, *end = start + path.size;

	u8 up = 0;
	for (u8* it = end - 1, *tail = end; it >= start; --it) {
		usz offs = 1;
		if (it == start)
			offs = 0;
		else if (*it != '/')
			continue;

		u8* name = it + offs;
		usz len = tail - name;
		if (len == 1 && name[0] == '.') {
			memmove(it, tail, end - tail);
			end -= len + offs;
		}
		else if (len == 2 && name[0] == '.' && name[1] == '.') {
			memmove(it, tail, end - tail);
			end -= len + offs;
			++up;
		}
		else if (up) {
			memmove(it, tail, end - tail);
			end -= len + offs;
			--up;
		}
		tail = it;
	}

	return lsrange(path.ptr, end);
}

static
void handle_request($async, server_info* server, client_state* state) {
	$enter_task();

	state->accepted_at_us = time_us();

#ifdef LT_OPENSSL
	if (state->is_https) {
		state->tls_handshake = (struct tls_handshake_state) {
			.context = server->tls_cx,
			.socket  = state->socket,
			.timeout_at_ms = time_ms() + S_TO_MS(8)
		};
		$awaitv(state->tls, $subtask, socket_accept_tls_async($subtask, &state->tls_handshake, err_warn));
		if (!state->tls) {
			llogf(server->logger, LOG_NOTICE, "failed to accept tls connection");
			goto end;
		}
	}
#endif

	state->http = (struct http_request_state) {
		.socket = state->socket,
		.tls    = state->tls,

		.buffer_start = state->recv_buf,
		.buffer_end   = state->recv_buf + sizeof(state->recv_buf),

		.max_header_count = COUNT_OF(state->header_values),
		.header_keys      = state->header_keys,
		.header_values    = state->header_values,

		.timeout_at_ms = time_ms() + S_TO_MS(16),
	};

	b8 request_valid;
	$awaitv(request_valid, $subtask, receive_http_request_async($subtask, &state->http, err_warn));
	if UNLIKELY (!request_valid) {
		$await($subtask, server->on_invalid_request($subtask, server, state));
		goto end;
	}

	llogf(server->logger, LOG_INFO, "{u8}.{u8}.{u8}.{u8} - {ls} {ls}{ls}", state->address.ip_addr[0], state->address.ip_addr[1], state->address.ip_addr[2], state->address.ip_addr[3], state->http.method, state->http.host, state->http.path);
	for (usz i = 0; i < state->http.header_count; ++i) {
		if (lseq_upper(state->http.header_keys[i], ls("USER-AGENT"))) {
			llogf(server->logger, LOG_INFO, "User-Agent: {ls}", state->http.header_values[i]);
			break;
		}
	}

	ls path = lssplit(state->http.path, '?');
	path = normalize_path(path);

	ls query = lsdrop(state->http.path, path.size);

	for (usz i = 0; i < server->mapping_count; ++i) {
		path_mapping* mapping = &server->mappings[i];
		if (!lseq(mapping->method, state->http.method) || (mapping->host.size && !lseq(mapping->host, state->http.host)))
			continue;

		if (mapping->path.size && lseq(mapping->path, path))
			state->mapped_path = mapping->real_path;
		else if (mapping->base_path.size && lsprefix(path, mapping->base_path))
			state->mapped_path = lsprintf(ls(state->mapped_path_buf), "{ls}{ls}", mapping->real_path, lsdrop(path, mapping->base_path.size));
		else
			continue;

		state->mapping = mapping;
		if (mapping->page)
			$await($subtask, send_page($subtask, server, state, state->mapping->page));
		else if (mapping->func)
			$await($subtask, state->mapping->func($subtask, server, state));
		else {
			state->real_path = state->mapped_path;
			$await($subtask, send_file($subtask, server, state, state->real_path));
		}
		goto end;
	}
	$await($subtask, server->on_unmapped_request($subtask, server, state));

end:
#ifdef LT_OPENSSL
	if (state->is_https)
		socket_close_tls(state->tls);
#endif
	socket_close(state->socket, err_warn);
	state->active = 0;
	llogf(server->logger, LOG_INFO, "response finished after {u64}us", time_us() - state->accepted_at_us);
}

static
void task_thread(server_info* server) {
	while (!server->cancel) {
		for (usz i = 0; i < server->max_client_count; ++i) {
			client_state* client = &server->clients[i];
			if (client->active)
				handle_request(client->task_stack, server, client);
		}
		sleep_us(250);
	}
}

static
void on_unmapped_request($async, server_info* server, client_state* state) {
	llogf(server->logger, LOG_NOTICE, "page not found");
	ls res = ls(
		"HTTP/1.1 404 Not Found\r\n"
		"Connection: close\r\n"
		"Content-Length: 10\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"Not Found\n");
	send_raw(state, res.ptr, res.size, err_warn);
}

static
void on_invalid_request($async, server_info* server, client_state* state) {
	llogf(server->logger, LOG_NOTICE, "invalid request");
	ls res = ls(
		"HTTP/1.1 400 Bad Request\r\n"
		"Connection: close\r\n"
		"Content-Length: 12\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"Bad Request\n");
	send_raw(state, res.ptr, res.size, err_warn);
}

static
void write_default_headers($async, server_info* server, client_state* state, usz size, ls mime_type) {
	u8 send_header_buf[1024];
	ls res = lsprintf(ls(send_header_buf),
		"HTTP/1.1 200 OK\r\n"
		"Connection: close\r\n"
		"Content-Length: {usz}\r\n"
		"Content-Type: {ls}\r\n"
		"\r\n",
		size, mime_type);
	send_raw(state, res.ptr, res.size, err_warn);
}

void serve_http(server_info* server, err* err) {
	signal(SIGPIPE, SIG_IGN);

	server->clients = malloc(sizeof(client_state) * server->max_client_count);
	if UNLIKELY (!server->clients) {
		throw(err, ERR_NO_MEMORY, "failed to allocate client states");
		return;
	}

	if (!server->on_invalid_request)
		server->on_invalid_request = on_invalid_request;
	if (!server->on_unmapped_request)
		server->on_unmapped_request = on_unmapped_request;
	if (!server->write_default_headers)
		server->write_default_headers = write_default_headers;

#ifdef LT_OPENSSL
	server->tls_cx = tls_load_certificates(server->cert_path, server->key_path, server->cert_chain_path, err_warn);

	server->https_socket = socket_open(SOCKET_TCP | SOCKET_ASYNC, err_warn);
	socket_bind(server->https_socket, server->https_port, err_warn);
#endif

	if (server->http_port) {
		server->http_socket = socket_open(SOCKET_TCP | SOCKET_ASYNC, err_warn);	
		socket_bind(server->http_socket, server->http_port, err_warn);
	}

	thread_spawn(&server->task_thread, (thread_fn)task_thread, server, err_warn);

	while (!server->cancel) {
		socket_addr addr;
		socket_handle client_socket;
		b8 is_https;

#ifdef LT_OPENSSL
		if (socket_readable(server->https_socket, 0)) {
			client_socket = socket_accept(server->https_socket, &addr, SOCKET_ASYNC, err_warn);
			if (client_socket < 0) {
				llogf(server->logger, LOG_NOTICE, "failed to accept https connection");
				continue;
			}
			is_https = 1;
		}
		else
#endif
		if (socket_readable(server->http_socket, 0)) {
			client_socket = socket_accept(server->http_socket, &addr, SOCKET_ASYNC, err_warn);
			if (client_socket < 0) {
				llogf(server->logger, LOG_NOTICE, "failed to accept http connection");
				continue;
			}
			is_https = 0;
		}
		else {
			sleep_us(250);
			continue;
		}

		for (usz i = 0; i < server->max_client_count; ++i) {
			client_state* client = &server->clients[i];
			if (client->active)
				continue;

			task* end = client->task_stack + COUNT_OF(client->task_stack);
			for (task* it = client->task_stack; it < end; ++it)
				*it = (struct task) { { 0 }, end };

			client->address     = addr;
			client->socket      = client_socket;
			client->tls         = NULL;
			client->real_path   = ls("");
			client->mapped_path = ls("");
			client->mapping     = NULL;
			client->is_https    = is_https;
			client->active      = 1;
			goto next;
		}

		llogf(server->logger, LOG_WARN, "client pool is full, rejecting connection...");
		socket_close(client_socket, err_warn);
	next:
	}

	if (server->http_port)
		socket_close(server->http_socket, err_warn);
	socket_close(server->https_socket, err_warn);
	thread_join(&server->task_thread, err_warn);

	free(server->clients);
}


