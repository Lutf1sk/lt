#ifndef LT_SPOTIFY_H
#define LT_SPOTIFY_H 1

#include <lt/fwd.h>
#include <lt/err.h>

typedef
struct lt_spotify {
	lt_socket_t* sock;
	lt_ssl_connection_t* conn;

	lstr_t client_id;
	lstr_t client_secret;
	lstr_t auth_code;
	lstr_t auth_token;
	lstr_t refresh_token;
} lt_spotify_t;

typedef
struct lt_spotify_device {
	char id[64];
	char name[64];
} lt_spotify_device_t;

typedef
struct lt_spotify_track {
	char id[32];
	char album[128];
	char name[128];
	char artists[256];
} lt_spotify_track_t;

lt_err_t lt_spotify_init(lt_spotify_t* spt, lstr_t client_id, lstr_t client_secret);
void lt_spotify_destroy(lt_spotify_t* spt, lt_alloc_t* alloc);

lt_err_t lt_spotify_request_auth_code(lt_spotify_t* spt, lt_alloc_t* alloc);
lt_err_t lt_spotify_create_token(lt_spotify_t* spt, lt_alloc_t* alloc);
lt_err_t lt_spotify_refresh_token(lt_spotify_t* spt, lt_alloc_t* alloc);

lt_err_t lt_spotify_connect(lt_spotify_t* spt, lt_alloc_t* alloc);
void lt_spotify_disconnect(lt_spotify_t* spt, lt_alloc_t* alloc);

lt_darr(lt_spotify_device_t) lt_spotify_device_list(lt_spotify_t* spt, lt_alloc_t* alloc);

lt_err_t lt_spotify_play_track(lt_spotify_t* spt, lstr_t device_id, lstr_t track_id, lt_alloc_t* alloc);
lt_err_t lt_spotify_queue_track(lt_spotify_t* spt, lstr_t device_id, lstr_t track_id, lt_alloc_t* alloc);

lt_err_t lt_spotify_get_track(lt_spotify_t* spt, lstr_t track_id, lt_spotify_track_t* out_track, lt_alloc_t* alloc);

#endif