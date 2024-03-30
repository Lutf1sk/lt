#ifdef LT_SSL

#include <lt/spotify.h>
#include <lt/ssl.h>
#include <lt/net.h>
#include <lt/shell.h>
#include <lt/io.h>
#include <lt/debug.h>
#include <lt/http.h>
#include <lt/mem.h>
#include <lt/str.h>
#include <lt/base64.h>
#include <lt/json.h>
#include <lt/darr.h>

#include <lt/internal.h>

#define AUTH_SCOPE "user-read-playback-state%20user-modify-playback-state%20user-read-currently-playing%20app-remote-control%20streaming"
#define AUTH_REDIRECT "http%3A%2F%2Flocalhost%3A7788"

#define ACCOUNTS_HOST "accounts.spotify.com"
#define ACCOUNTS_PORT 443

#define API_HOST "api.spotify.com"
#define API_PORT 443

static
lt_err_t spotify_request(lt_spotify_t* spt, char* method, char* endpoint, char* headers, lstr_t body, lt_http_msg_t* out_res, lt_alloc_t* alloc) {
	lt_err_t err;

retry:
	// !!
	char buf[4096];
	isz len = lt_sprintf(buf,
			"%s %s HTTP/1.1\r\n"
			"Host: %s\r\n"
			"Connection: keep-alive\r\n"
			"%s"
			"Content-Length: %uz\r\n"
			"Authorization: Bearer %S\r\n"
			"\r\n"
			"%S",
			method, endpoint, API_HOST, headers, body.len, spt->auth_token, body);

	isz res = lt_ssl_send_fixed(spt->conn, buf, len);
	if (res < 0) {
		lt_http_msg_destroy(out_res, alloc);
		if (res == -LT_ERR_CLOSED)
			goto reconnect;
		return -res;
	}

	if ((err = lt_http_parse_response(out_res, (lt_io_callback_t)lt_ssl_recv_fixed, spt->conn, alloc))) {
		lt_http_msg_destroy(out_res, alloc);
		if (err == LT_ERR_CLOSED)
			goto reconnect;
		return err;
	}

	if (out_res->response_status_code == 401) {
		lt_http_msg_destroy(out_res, alloc);
		if ((err = lt_spotify_refresh_token(spt, alloc)))
			return err;
		goto retry;
	}

// 	lt_printf("%s %s returned %uw %S\n", method, endpoint, out_res->status_code, out_res->status_msg);
	return LT_SUCCESS;

reconnect:	lt_spotify_disconnect(spt, alloc);
			if ((err = lt_spotify_connect(spt, alloc)))
				return err;
			goto retry;
}

lt_err_t lt_spotify_init(lt_spotify_t* spt, lstr_t client_id, lstr_t client_secret) {
	spt->sock = NULL;
	spt->conn = NULL;
	spt->client_id = client_id;
	spt->client_secret = client_secret;
	spt->auth_code = NLSTR();
	spt->auth_token = NLSTR();
	spt->refresh_token = NLSTR();

	return LT_SUCCESS;
}

void lt_spotify_destroy(lt_spotify_t* spt, lt_alloc_t* alloc) {
	if (spt->conn)
		lt_ssl_connection_destroy(spt->conn);
	if (spt->sock)
		lt_socket_destroy(spt->sock, alloc);

	if (spt->auth_code.str)
		lt_mfree(alloc, spt->auth_code.str);
	if (spt->auth_token.str)
		lt_mfree(alloc, spt->auth_token.str);
	if (spt->refresh_token.str)
		lt_mfree(alloc, spt->refresh_token.str);
}

lt_err_t lt_spotify_request_auth_code(lt_spotify_t* spt, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_socket_t* sock = lt_socket_create(LT_SOCKTYPE_TCP, alloc);
	if (!sock)
		return LT_ERR_UNKNOWN;

	if ((err = lt_socket_server(sock, 7788)))
		goto err0;

	lstr_t cmd = lt_lsbuild(alloc, "xdg-open 'https://accounts.spotify.com/authorize?response_type=code&client_id=%S&scope=%s&redirect_uri=%s'", spt->client_id, AUTH_SCOPE, AUTH_REDIRECT);
	if (!cmd.str)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err0);
	if ((err = lt_shell_exec(cmd, NULL)))
		goto err1;

	lt_socket_t* client = lt_socket_accept(sock, NULL, alloc);
	if (!client)
		fail_to(err = LT_ERR_UNKNOWN, err1);

	// !!
	char request_buf[8192];
	isz request_len = lt_socket_recv(client, request_buf, sizeof(request_buf));
	if (request_len < 0)
		fail_to(err = -request_len, err1);

	// !!
	char* start = request_buf + CLSTR("GET /?code=").len, *it = start;
	while (*it != ' ')
		++it;

	lstr_t response = CLSTR(
			"HTTP/1.1 200 OK\r\n"
			"Connection: close\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 31\r\n"
			"\r\n"
			"<script>window.close()</script>");
	isz sent_len = lt_socket_send(client, response.str, response.len);
	if (sent_len < 0)
		fail_to(err = -sent_len, err2);

	spt->auth_code = lt_strdup(alloc, lt_lsfrom_range(start, it));
	if (!spt->auth_code.str)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err2);
	err = LT_SUCCESS;

err2:	lt_socket_destroy(client, alloc);
err1:	lt_mfree(alloc, cmd.str);
err0:	lt_socket_destroy(sock, alloc);
		return err;
}

lt_err_t lt_spotify_create_token(lt_spotify_t* spt, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_sockaddr_t sockaddr;
	if ((err = lt_sockaddr_resolve(CLSTR(ACCOUNTS_HOST), ACCOUNTS_PORT, LT_SOCKTYPE_TCP, &sockaddr, alloc)))
		return err;

	lt_socket_t* sock = lt_socket_create(LT_SOCKTYPE_TCP, alloc);
	if (!sock)
		return LT_ERR_UNKNOWN;
	if ((err = lt_socket_connect(sock, &sockaddr)))
		goto err0;

	lt_ssl_connection_t* conn = lt_ssl_connect(sock, CLSTR(ACCOUNTS_HOST));
	if (!conn)
		fail_to(err = LT_ERR_UNKNOWN, err0);

	// !!
	char unenc_auth_buf[512];
	lstr_t unenc_auth = LSTR(unenc_auth_buf, lt_sprintf(unenc_auth_buf, "%S:%S", spt->client_id, spt->client_secret));
	char tmpauth_buf[512];
	lstr_t tmpauth = LSTR(tmpauth_buf, lt_base64_encode(unenc_auth, tmpauth_buf));

	lstr_t req_body = lt_lsbuild(alloc, "grant_type=authorization_code&code=%S&redirect_uri=%s&client_id=%S", spt->auth_code, AUTH_REDIRECT, spt->client_id);
	if (!req_body.str)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err1);
	lstr_t req = lt_lsbuild(alloc,
			"POST /api/token HTTP/1.1\r\n"
			"Host: "ACCOUNTS_HOST"\r\n"
			"Connection: close\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Authorization: Basic %S\r\n"
			"Content-Length: %uz\r\n"
			"\r\n"
			"%S",
			tmpauth, req_body.len, req_body);
	if (!req.str)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err2);

	isz res = lt_ssl_send_fixed(conn, req.str, req.len);
	if (res < 0)
		fail_to(err = -res, err3);

	lt_http_msg_t response;
	if ((err = lt_http_parse_response(&response, (lt_io_callback_t)lt_ssl_recv_fixed, conn, alloc)))
		goto err3;
	if (response.response_status_code != 200)
		fail_to(err = LT_ERR_UNKNOWN, err4);

	lt_json_t* body_json = lt_json_parse(alloc, response.body.str, response.body.len);
	if (!body_json)
		fail_to(err = LT_ERR_UNKNOWN, err4);

	lt_json_t* access_token_json = lt_json_find_child(body_json, CLSTR("access_token"));
	lt_json_t* token_type_json = lt_json_find_child(body_json, CLSTR("token_type"));
	lt_json_t* refresh_token_json = lt_json_find_child(body_json, CLSTR("refresh_token"));

	if (!access_token_json || access_token_json->stype != LT_JSON_STRING)
		fail_to(err = LT_ERR_UNKNOWN, err5);
	if (!token_type_json || token_type_json->stype != LT_JSON_STRING || !lt_lseq_nocase(token_type_json->str_val, CLSTR("Bearer")))
		fail_to(err = LT_ERR_UNKNOWN, err5);
	if (!refresh_token_json || refresh_token_json->stype != LT_JSON_STRING)
		fail_to(err = LT_ERR_UNKNOWN, err5);

	lstr_t auth_token = lt_strdup(alloc, access_token_json->str_val);
	if (!auth_token.str)
		fail_to(err = LT_ERR_UNKNOWN, err5);
	lstr_t refresh_token = lt_strdup(alloc, refresh_token_json->str_val);
	if (!refresh_token.str) {
		lt_mfree(alloc, auth_token.str);
		fail_to(err = LT_ERR_UNKNOWN, err5);
	}

	spt->auth_token = auth_token;
	spt->refresh_token = refresh_token;
	err = LT_SUCCESS;

err5:	// lt_json_free(body_json, alloc);
err4:	lt_http_msg_destroy(&response, alloc);
err3:	lt_mfree(alloc, req.str);
err2:	lt_mfree(alloc, req_body.str);
err1:	lt_ssl_connection_destroy(conn);
err0:	lt_socket_destroy(sock, alloc);
		return err;
}

lt_err_t lt_spotify_refresh_token(lt_spotify_t* spt, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_mfree(alloc, spt->auth_token.str);
	spt->auth_token = NLSTR();

	lt_sockaddr_t sockaddr;
	if ((err = lt_sockaddr_resolve(CLSTR(ACCOUNTS_HOST), ACCOUNTS_PORT, LT_SOCKTYPE_TCP, &sockaddr, alloc)))
		return err;

	lt_socket_t* sock = lt_socket_create(LT_SOCKTYPE_TCP, alloc);
	if (!sock)
		return LT_ERR_UNKNOWN;
	if ((err = lt_socket_connect(sock, &sockaddr)))
		goto err0;

	lt_ssl_connection_t* conn = lt_ssl_connect(sock, CLSTR(ACCOUNTS_HOST));
	if (!conn)
		fail_to(err = LT_ERR_UNKNOWN, err0);

	// !!
	char unenc_auth_buf[512];
	lstr_t unenc_auth = LSTR(unenc_auth_buf, lt_sprintf(unenc_auth_buf, "%S:%S", spt->client_id, spt->client_secret));
	char tmpauth_buf[512];
	lstr_t tmpauth = LSTR(tmpauth_buf, lt_base64_encode(unenc_auth, tmpauth_buf));

	lstr_t req_body = lt_lsbuild(alloc, "grant_type=refresh_token&refresh_token=%S&", spt->refresh_token, AUTH_REDIRECT, spt->client_id);
	if (!req_body.str)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err1);
	lstr_t req = lt_lsbuild(alloc,
			"POST /api/token HTTP/1.1\r\n"
			"Host: "ACCOUNTS_HOST"\r\n"
			"Connection: close\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Authorization: Basic %S\r\n"
			"Content-Length: %uz\r\n"
			"\r\n"
			"%S",
			tmpauth, req_body.len, req_body);
	if (!req.str)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err2);

	isz res = lt_ssl_send_fixed(conn, req.str, req.len);
	if (res < 0)
		fail_to(err = -res, err3);

	lt_http_msg_t response;
	if ((err = lt_http_parse_response(&response, (lt_io_callback_t)lt_ssl_recv_fixed, conn, alloc)))
		goto err3;
	if (response.response_status_code != 200)
		fail_to(err = LT_ERR_UNKNOWN, err4);

	lt_json_t* body_json = lt_json_parse(alloc, response.body.str, response.body.len);
	if (!body_json)
		fail_to(err = LT_ERR_UNKNOWN, err4);

	lt_json_t* access_token_json = lt_json_find_child(body_json, CLSTR("access_token"));
	lt_json_t* token_type_json = lt_json_find_child(body_json, CLSTR("token_type"));

	if (!access_token_json || access_token_json->stype != LT_JSON_STRING)
		fail_to(err = LT_ERR_UNKNOWN, err5);
	if (!token_type_json || token_type_json->stype != LT_JSON_STRING || !lt_lseq_nocase(token_type_json->str_val, CLSTR("Bearer")))
		fail_to(err = LT_ERR_UNKNOWN, err5);

	lstr_t auth_token = lt_strdup(alloc, access_token_json->str_val);
	if (!auth_token.str)
		fail_to(err = LT_ERR_UNKNOWN, err5);

	spt->auth_token = auth_token;
	err = LT_SUCCESS;

err5:	// lt_json_free(body_json, alloc);
err4:	lt_http_msg_destroy(&response, alloc);
err3:	lt_mfree(alloc, req.str);
err2:	lt_mfree(alloc, req_body.str);
err1:	lt_ssl_connection_destroy(conn);
err0:	lt_socket_destroy(sock, alloc);
		return err;
}

lt_err_t lt_spotify_connect(lt_spotify_t* spt, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_sockaddr_t sockaddr;
	if ((err = lt_sockaddr_resolve(CLSTR(API_HOST), API_PORT, LT_SOCKTYPE_TCP, &sockaddr, alloc)))
		return err;

	if (!(spt->sock = lt_socket_create(LT_SOCKTYPE_TCP, alloc)))
		return LT_ERR_UNKNOWN;
	if ((err = lt_socket_connect(spt->sock, &sockaddr)))
		goto err0;
	if (!(spt->conn = lt_ssl_connect(spt->sock, CLSTR(API_HOST))))
		fail_to(err = LT_ERR_UNKNOWN, err0);

	return LT_SUCCESS;

err0:	lt_socket_destroy(spt->sock, alloc);
		return err;
}

void lt_spotify_disconnect(lt_spotify_t* spt, lt_alloc_t* alloc) {
	lt_ssl_connection_destroy(spt->conn);
	lt_socket_destroy(spt->sock, alloc);
	spt->conn = NULL;
	spt->sock = NULL;
}

lt_darr(lt_spotify_device_t) lt_spotify_device_list(lt_spotify_t* spt, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "GET", "/v1/me/player/devices", "", NLSTR(), &res, alloc)))
		return NULL;

	lt_json_t* body_json = lt_json_parse(alloc, res.body.str, res.body.len);
	if (!body_json || body_json->stype != LT_JSON_OBJECT)
		goto err0;

	lt_darr(lt_spotify_device_t) arr = lt_darr_create(lt_spotify_device_t, 4, alloc);
	if (!arr)
		goto err0;

	lt_json_t* devices_json = lt_json_find_child(body_json, CLSTR("devices"));
	if (!devices_json || devices_json->stype != LT_JSON_ARRAY)
		goto err1;

	for (lt_json_t* child_it = devices_json->child; child_it; child_it = child_it->next) {
		if (child_it->stype != LT_JSON_OBJECT)
			continue;

		lt_spotify_device_t dev;

		lt_json_t* id_json = lt_json_find_child(child_it, CLSTR("id"));
		if (!id_json || id_json->stype != LT_JSON_STRING || id_json->str_val.len >= sizeof(dev.id))
			continue;
		lt_json_t* name_json = lt_json_find_child(child_it, CLSTR("name"));
		if (!name_json || name_json->stype != LT_JSON_STRING || name_json->str_val.len >= sizeof(dev.name))
			continue;

		memcpy(dev.id, id_json->str_val.str, id_json->str_val.len);
		dev.id[id_json->str_val.len] = 0;
		memcpy(dev.name, name_json->str_val.str, name_json->str_val.len);
		dev.name[name_json->str_val.len] = 0;

		lt_darr_push(arr, dev);
	}

	// lt_json_free(body_json, alloc);
	lt_http_msg_destroy(&res, alloc);
	return arr;

// err2:	// lt_json_free(body_json, alloc);
err1:	lt_darr_destroy(arr);
err0:	lt_http_msg_destroy(&res, alloc);
		return NULL;
}

lt_err_t lt_spotify_play_track(lt_spotify_t* spt, lstr_t device_id, lstr_t track_id, lt_alloc_t* alloc) {
	lt_err_t err;

	// !!
	char buf[4096];
	usz len = lt_sprintf(buf,
		"{"
		"	\"uris\": [ \"spotify:track:%S\" ],"
		"	\"position_ms\": 0"
		"}",
		track_id);

	// !!
	char buf2[4096];
	buf2[lt_sprintf(buf2, "/v1/me/player/play?device_id=%S", device_id)] = 0;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "PUT", buf2, "Content-Type: application/json\r\n", LSTR(buf, len), &res, alloc)))
		return err;
	lt_http_msg_destroy(&res, alloc);

	return LT_SUCCESS;
}

lt_err_t lt_spotify_next_track(lt_spotify_t* spt, lstr_t device_id, lt_alloc_t* alloc) {
	lt_err_t err;

	// !!
	char buf[4096];
	buf[lt_sprintf(buf, "/v1/me/player/next?&device_id=%S", device_id)] = 0;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "POST", buf, "", NLSTR(), &res, alloc)))
		return err;

	lt_http_msg_destroy(&res, alloc);
	return LT_SUCCESS;
}

lt_err_t lt_spotify_prev_track(lt_spotify_t* spt, lstr_t device_id, lt_alloc_t* alloc) {
	lt_err_t err;

	// !!
	char buf[4096];
	buf[lt_sprintf(buf, "/v1/me/player/previous?&device_id=%S", device_id)] = 0;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "POST", buf, "", NLSTR(), &res, alloc)))
		return err;

	lt_http_msg_destroy(&res, alloc);
	return LT_SUCCESS;
}

lt_err_t lt_spotify_queue_track(lt_spotify_t* spt, lstr_t device_id, lstr_t track_id, lt_alloc_t* alloc) {
	lt_err_t err;

	// !!
	char buf[4096];
	buf[lt_sprintf(buf, "/v1/me/player/queue?uri=spotify%%3Atrack%%3A%S&device_id=%S", track_id, device_id)] = 0;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "POST", buf, "", NLSTR(), &res, alloc)))
		return err;

	lt_http_msg_destroy(&res, alloc);
	return LT_SUCCESS;
}

static
lt_err_t track_from_json(lt_json_t* json, lt_spotify_track_t* out_track) {
	lt_json_t* name_json = lt_json_find_child(json, CLSTR("name"));
	lt_json_t* album_json = lt_json_find_child(json, CLSTR("album"));
	lt_json_t* album_name_json = lt_json_find_child(album_json, CLSTR("name"));
	lt_json_t* artists_json = lt_json_find_child(json, CLSTR("artists"));

	if (!album_json || !album_name_json || !name_json || !artists_json)
		return LT_ERR_NOT_FOUND;
	if (album_json->stype != LT_JSON_OBJECT || name_json->stype != LT_JSON_STRING || album_name_json->stype != LT_JSON_STRING || artists_json->stype != LT_JSON_ARRAY)
		return LT_ERR_INVALID_TYPE;

	lstr_t album = album_name_json->str_val;
	lstr_t name = name_json->str_val;

	if (album.len >= sizeof(out_track->album))
		return LT_ERR_UNKNOWN;
	memcpy(out_track->album, album.str, album.len);
	out_track->album[album.len] = 0;

	if (name.len >= sizeof(out_track->name))
		return LT_ERR_UNKNOWN;
	memcpy(out_track->name, name.str, name.len);
	out_track->name[name.len] = 0;

	// !! it goes out of range
	char* it = out_track->artists, *end = it + sizeof(out_track->artists) - 1;
	for (lt_json_t* child_it = artists_json->child; child_it; child_it = child_it->next) {
		if (child_it->stype != LT_JSON_OBJECT)
			continue;

		lt_json_t* name_json = lt_json_find_child(child_it, CLSTR("name"));
		if (!name_json || name_json->stype != LT_JSON_STRING)
			continue;

		memcpy(it, name_json->str_val.str, name_json->str_val.len);
		it += name_json->str_val.len;

		if (child_it->next) {
			*it++ = ',';
			*it++ = ' ';
		}
	}
	*it = 0;
	return LT_SUCCESS;
}

lt_err_t lt_spotify_current_track(lt_spotify_t* spt, lt_spotify_track_t* out_track, lt_alloc_t* alloc) {
	lt_err_t err;

	// !!
	char buf[4096];
	buf[lt_sprintf(buf, "/v1/me/player")] = 0;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "GET", buf, "", NLSTR(), &res, alloc)))
		return err;

	lt_json_t* body_json = lt_json_parse(alloc, res.body.str, res.body.len);
	if (!body_json || body_json->stype != LT_JSON_OBJECT)
		fail_to(err = LT_ERR_INVALID_FORMAT, err0);

	if ((err = track_from_json(lt_json_find_child(body_json, CLSTR("item")), out_track)))
		goto err0;

	err = LT_SUCCESS;

// err1:	// lt_json_free(body_json, alloc);
err0:	lt_http_msg_destroy(&res, alloc);
		return LT_ERR_UNKNOWN;
}

lt_err_t lt_spotify_get_track(lt_spotify_t* spt, lstr_t track_id, lt_spotify_track_t* out_track, lt_alloc_t* alloc) {
	lt_err_t err;

	// !!
	char buf[4096];
	buf[lt_sprintf(buf, "/v1/tracks/%S", track_id)] = 0;

	lt_http_msg_t res;
	if ((err = spotify_request(spt, "GET", buf, "", NLSTR(), &res, alloc)))
		return err;

	lt_json_t* body_json = lt_json_parse(alloc, res.body.str, res.body.len);
	if (!body_json || body_json->stype != LT_JSON_OBJECT)
		fail_to(err = LT_ERR_INVALID_FORMAT, err0);

	if ((err = track_from_json(body_json, out_track)))
		goto err0;

	err = LT_SUCCESS;

// err1:	// lt_json_free(body_json, alloc);
err0:	lt_http_msg_destroy(&res, alloc);
		return LT_ERR_UNKNOWN;
}


#endif