#ifndef LT_AUDIO_H
#define LT_AUDIO_H 1

#include <lt/fwd.h>
#include <lt/err.h>

typedef
enum lt_audio_stream_flags {
	LT_AUDIO_STREAM_READ = 0x01,
	LT_AUDIO_STREAM_WRITE = 0x02,
} lt_audio_stream_flags_t;

typedef
enum lt_audio_format {
	LT_AUDIO_I8,
	LT_AUDIO_I16,
	LT_AUDIO_I24,
	LT_AUDIO_I32,
} lt_audio_format_t;

typedef
struct lt_audio_stream {
	u32 rate;
	u16 channels, format;

#if defined(LT_UNIX)
#	define LT_AUDIO_MAX_POLLFDS 8

	usz buffer_frames;
	void* handle;
	usz pollfd_count;
	void* pollfds;
#endif
} lt_audio_stream_t;

typedef void(*lt_audio_stream_fn_t)(lt_audio_stream_t s[static 1], void* frames, usz frame_count);

lt_err_t lt_audio_stream_create(lt_audio_stream_t s[static 1], u32 rate, u32 channels, lt_audio_stream_flags_t flags, lt_alloc_t alloc[static 1]);
void lt_audio_stream_destroy(lt_audio_stream_t s[static 1], lt_alloc_t alloc[static 1]);

lt_err_t lt_audio_stream_start(lt_audio_stream_t s[static 1]);
lt_err_t lt_audio_stream_stop(lt_audio_stream_t s[static 1]);
lt_err_t lt_audio_stream_drain(lt_audio_stream_t s[static 1]);
lt_err_t lt_audio_stream_pause(lt_audio_stream_t s[static 1]);
lt_err_t lt_audio_stream_resume(lt_audio_stream_t s[static 1]);

lt_err_t lt_audio_stream_wait(lt_audio_stream_t s[static 1]);

isz lt_audio_stream_write(lt_audio_stream_t s[static 1], void* data, usz frame_count);
isz lt_audio_stream_read(lt_audio_stream_t s[static 1], void* data, usz frame_count);

typedef
struct lt_audio {
	usz frame_count;
	usz bytes_per_frame, bytes_per_sample;
	u16 channels, format;
	u32 rate;
	void* data;
} lt_audio_t;

lt_err_t lt_audio_load_wav(lt_audio_t audio[static 1], void* data, usz len, lt_alloc_t alloc[static 1]);
lt_err_t lt_audio_load(lt_audio_t audio[static 1], void* data, usz len, lt_alloc_t alloc[static 1]);

#endif