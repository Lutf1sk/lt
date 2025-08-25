#pragma once

#include <lt2/common.h>
#include <lt2/net.h>
#include <lt2/http.h>
#include <lt2/thread.h>
#include <lt2/page.h>

typedef struct server_info  server_info;
typedef struct client_state client_state;
typedef struct path_mapping path_mapping;

typedef struct client_state {
	u8 recv_buf[MB(1)];
	u8 page_buf[KB(64)];
	u8 mapped_path_buf[1024];
	u8 real_path_buf[1024];

	ls mapped_path;
	ls real_path;

	ls header_keys[256];
	ls header_values[256];

	u64 accepted_at_us;

	socket_addr address;

	socket_handle socket;
	tls_handle* tls;

	tls_handshake_state tls_handshake;
	http_request_state http;
	task task_stack[8];

	path_mapping* mapping;

	b8 is_https;
	_Atomic volatile b8 active;
} client_state;

typedef struct path_mapping {
	ls host;
	ls method;
	ls path;
	ls base_path;
	ls real_path;
	ls mime_type;
	void (*page)($template);
	void (*func)($async, server_info* server, client_state* state);
	u64 flags;
} path_mapping;

typedef struct server_info {
	ls cert_path;
	ls key_path;
	ls cert_chain_path;

	u16 http_port;
	socket_handle http_socket;

	u16 https_port;
	tls_context* tls_cx;
	socket_handle https_socket;

	client_state* clients;
	usz max_client_count;

	thread_handle task_thread;
	_Atomic volatile b8 cancel;

	path_mapping* mappings;
	usz mapping_count;

	void (*on_invalid_request)($async, server_info* server, client_state* client);
	void (*on_unmapped_request)($async, server_info* server, client_state* client);
	void (*write_default_headers)($async, server_info* server, client_state* client, usz size, ls mime_type);
} server_info;

ls mime_type_or_default(ls path, ls default_);
ls mime_type_from_ext(ls path);

usz send_raw(client_state* state, void* data, usz size, err* error);
void send_page($async, server_info* server, client_state* state, void (*page)($template));
void send_file($async, server_info* server, client_state* state, ls real_path);

ls normalize_path(ls path);

void serve_http(server_info* server);

