#include <lt/audio.h>
#include <lt/mem.h>
#include <lt/io.h>

typedef
LT_PACKED_STRUCT(lt_wav_header) {
	char sign[4];
	u32 size;
} lt_wav_header_t;

#define LT_WAV_FMT_PCM 1

typedef
LT_PACKED_STRUCT(lt_wav_fmtheader) {
	lt_wav_header_t header;
	u16 audio_format;
	u16 channels;
	u32 sample_rate;
	u32 bytes_per_second;
	u16 bytes_per_frame;
	u16 bits_per_sample;
} lt_wav_fmtheader_t;

typedef
LT_PACKED_STRUCT(lt_wav_dataheader) {
	lt_wav_header_t header;
	u8 data[];
} lt_wav_dataheader_t;

typedef
LT_PACKED_STRUCT(lt_wav_fileheader) {
	lt_wav_header_t header;
	char wave_sign[4];
} lt_wav_fileheader_t;

lt_err_t lt_audio_load_wav(lt_audio_t* audio, void* data, usz len, lt_alloc_t* alloc) {
	lt_wav_fileheader_t* fh = data;
	if (len < sizeof(lt_wav_fileheader_t) || memcmp(fh->header.sign, "RIFF", 4) != 0 || memcmp(fh->wave_sign, "WAVE", 4) != 0)
		return LT_ERR_INVALID_FORMAT;

	u16 channels = 2;
	u16 sample_rate = 44100;
	u16 bits_per_sample = 16;

	lt_wav_fmtheader_t* fmt = NULL;
	lt_wav_dataheader_t* dh = NULL;

	for (u8* it = (u8*)data + sizeof(lt_wav_fileheader_t), *end = it + fh->header.size - sizeof(lt_wav_fileheader_t); it < end;) {
		if (it + sizeof(lt_wav_header_t) > end)
			return LT_ERR_INVALID_FORMAT;

		lt_wav_header_t* h = (lt_wav_header_t*)it;

		if (memcmp(h->sign, "fmt ", 4) == 0) {
			fmt = (lt_wav_fmtheader_t*)h;
			if (fmt->audio_format != LT_WAV_FMT_PCM)
				return LT_ERR_INVALID_FORMAT;

			channels = fmt->channels;
			sample_rate = fmt->sample_rate;
			bits_per_sample = fmt->bits_per_sample;
		}

		else if (memcmp(h->sign, "data", 4) == 0) {
			dh = (lt_wav_dataheader_t*)h;
		}

		else
			lt_printf("unknown wav header signature '%S'\n", LSTR(h->sign, 4));

		it += h->size + sizeof(lt_wav_header_t);
	}

	if (!dh)
		return LT_ERR_INVALID_FORMAT;

	usz sample_fmt;
	switch (bits_per_sample) {
	case 8: sample_fmt = LT_AUDIO_I8; break;
	case 16: sample_fmt = LT_AUDIO_I16; break;
	case 24: sample_fmt = LT_AUDIO_I24; break;
	case 32: sample_fmt = LT_AUDIO_I32; break;
	default:
		return LT_ERR_INVALID_FORMAT;
	}

	usz src_bytes = dh->header.size;
	u8* src_data = dh->data;
	usz bytes_per_sample = bits_per_sample / 8;
	usz bytes_per_frame = bytes_per_sample * channels;
	usz frame_count = src_bytes / bytes_per_frame;

	if (src_data + src_bytes > (u8*)data + len)
		return LT_ERR_INVALID_FORMAT;

	u8* dst_data = lt_malloc(alloc, src_bytes);
	if (!dst_data)
		return LT_ERR_OUT_OF_MEMORY;

	memcpy(dst_data, src_data, src_bytes);

	audio->format = sample_fmt;
	audio->channels = channels;
	audio->rate = sample_rate;
	audio->frame_count = frame_count;
	audio->data = dst_data;
	audio->bytes_per_frame = bytes_per_frame;
	audio->bytes_per_sample = bytes_per_sample;

	return LT_SUCCESS;
}
