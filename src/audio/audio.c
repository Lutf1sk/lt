#include <lt/audio.h>
#include <lt/debug.h>
#include <lt/io.h>
#include <lt/thread.h>
#include <lt/mem.h>
#include <lt/internal.h>

#include <alsa/asoundlib.h>

lt_err_t lt_audio_stream_create(lt_audio_stream_t* s, u32 rate, u32 channels, lt_audio_stream_flags_t flags, lt_alloc_t* alloc) {
	lt_err_t err;

	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
	char* pcm_name = strdup("pulse");
	if (snd_pcm_open((snd_pcm_t**)&s->handle, pcm_name, stream, 0) < 0)
		return LT_ERR_NOT_FOUND;

	if (snd_pcm_nonblock(s->handle, 1) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);

	unsigned int periods = 4;
	snd_pcm_uframes_t period_size = 8192;

	snd_pcm_hw_params_t* hwparams;
	snd_pcm_hw_params_alloca(&hwparams);
	if (snd_pcm_hw_params_any(s->handle, hwparams) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);

	s->rate = rate;
	s->buffer_frames = (period_size * periods) >> 2;

	if (snd_pcm_hw_params_set_access(s->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);
	if (snd_pcm_hw_params_set_format(s->handle, hwparams, SND_PCM_FORMAT_S32_LE) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);
	if (snd_pcm_hw_params_set_rate_near(s->handle, hwparams, &s->rate, 0) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);
	if (snd_pcm_hw_params_set_channels(s->handle, hwparams, 2) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);
	if (snd_pcm_hw_params_set_periods(s->handle, hwparams, periods, 0) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);
	if (snd_pcm_hw_params_set_buffer_size(s->handle, hwparams, s->buffer_frames) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);

	if (snd_pcm_hw_params(s->handle, hwparams) < 0)
		fail_to(err = LT_ERR_UNKNOWN, err0);

	s->pollfd_count = snd_pcm_poll_descriptors_count(s->handle);
	s->pollfds = lt_malloc(alloc, s->pollfd_count * sizeof(struct pollfd));
	if (!s->pollfds)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err0);
	s->pollfd_count = snd_pcm_poll_descriptors(s->handle, s->pollfds, s->pollfd_count);

	return LT_SUCCESS;

err0:	snd_pcm_close(s->handle);
		return err;
}

void lt_audio_stream_destroy(lt_audio_stream_t* s, lt_alloc_t* alloc) {
	lt_mfree(alloc, s->pollfds);
	snd_pcm_close(s->handle);
}

lt_err_t lt_audio_stream_start(lt_audio_stream_t* s) {
	if (snd_pcm_start(s->handle) < 0)
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_audio_stream_stop(lt_audio_stream_t* s) {
	if (snd_pcm_drop(s->handle) < 0)
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_audio_stream_drain(lt_audio_stream_t* s) {
	if (snd_pcm_drain(s->handle) < 0)
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_audio_stream_pause(lt_audio_stream_t* s) {
	if (snd_pcm_resume(s->handle) < 0)
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_audio_stream_resume(lt_audio_stream_t* s) {
	if (snd_pcm_resume(s->handle) < 0)
		return LT_ERR_UNKNOWN;
	return LT_SUCCESS;
}

lt_err_t lt_audio_stream_wait(lt_audio_stream_t* s) {
	int res = poll(s->pollfds, s->pollfd_count, -1);
	if (res < 0)
		return LT_ERR_INTERRUPTED;

	LT_ASSERT(res);

	for (usz i = 0; i < s->pollfd_count; ++i) {
		if (((struct pollfd*)s->pollfds)[i].revents > 0)
			return LT_SUCCESS;
	}
	return LT_ERR_NOT_FOUND;
}

isz lt_audio_stream_write(lt_audio_stream_t* s, void* data, usz frame_count) {
	isz res = snd_pcm_writei(s->handle, data, frame_count);
	if (res < 0) {
		if (res == -EPIPE) {
			snd_pcm_prepare(s->handle);
			return -LT_ERR_UNDERFLOW;
		}
		return -LT_ERR_UNKNOWN;
	}
	return res;
}

isz lt_audio_stream_read(lt_audio_stream_t* s, void* data, usz frame_count) {
	snd_pcm_readi(s->handle, data, frame_count);
	return LT_SUCCESS;
}


lt_err_t lt_audio_load(lt_audio_t* audio, void* data, usz len, lt_alloc_t* alloc) {
	return lt_audio_load_wav(audio, data, len, alloc);
}
