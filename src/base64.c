#include <lt2/common.h>

static const u8 b64_enc_tab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const u8 b64_dec_tab[256] = {
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

usz b64_encoded_len(usz size) {
	usz rem = size % 3;
	usz enc = (size - rem) / 3 * 4; // !! can overflow
	if (rem == 1 || rem == 2)
		enc += 4;
	return enc;
}

usz b64_encode(void* dst_, void* src_, usz src_size) {
	u8* dst = dst_;
	u8* src = src_;

	u8* oit = dst;

	usz pass3_rem = src_size % 3;
	usz pass3_count = src_size - pass3_rem;
	for (usz i = 0; i < pass3_count; i += 3) {
		u8* data = &src[i];
		u8 c1 = data[0],
		   c2 = data[1],
		   c3 = data[2];

		*oit++ = b64_enc_tab[c1 >> 2];
		*oit++ = b64_enc_tab[((c1 & 0b000011) << 4) | (c2 >> 4)];
		*oit++ = b64_enc_tab[((c2 & 0b001111) << 2) | (c3 >> 6)];
		*oit++ = b64_enc_tab[c3 & 0b111111];
	}

	u8* rem_data = &src[pass3_count];
	if (pass3_rem == 1) {
		u8 c1 = rem_data[0];

		*oit++ = b64_enc_tab[c1 >> 2];
		*oit++ = b64_enc_tab[(c1 & 0b000011) << 4];
		*oit++ = '=';
		*oit++ = '=';
	}
	else if (pass3_rem == 2) {
		u8 c1 = rem_data[0],
		   c2 = rem_data[1];

		*oit++ = b64_enc_tab[c1 >> 2];
		*oit++ = b64_enc_tab[((c1 & 0b000011) << 4) | (c2 >> 4)];
		*oit++ = b64_enc_tab[(c2 & 0b001111) << 2];
		*oit++ = '=';
	}

	return oit - dst;
}

usz b64_decoded_len(void* src_, usz size, err* error) {
	u8* src = src_;

	usz pad = 0;
	while (pad < 3 && src[size - pad - 1] == '=')
		++pad;

	if (pad > 2) {
		throw(error, ERR_BAD_FORMAT, "invalid base64 padding length");
		return 0;
	}

	usz dec = ((size - pad) & ~0b11) / 4 * 3;
	if (pad == 2)
		dec += 1;
	if (pad == 1)
		dec += 2;
	return dec;
}

usz b64_decode(void* dst_, void* src_, usz src_size, err* error) {
	u8* dst = dst_;
	u8* src = src_;

	usz pad = 0;
	while (pad < 3 && src[src_size - pad - 1] == '=')
		++pad;

	if (pad > 2)
		return 0;

	usz pass4_count = (src_size - pad) & ~0b11;
	u8* oit = dst;

	for (usz i = 0; i < pass4_count; i += 4) {
		u8* data = (u8*)&src[i];
		u8 c1 = b64_dec_tab[data[0]],
		   c2 = b64_dec_tab[data[1]],
		   c3 = b64_dec_tab[data[2]],
		   c4 = b64_dec_tab[data[3]];

		*oit++ = (c1 << 2) | (c2 >> 4);
		*oit++ = (c2 << 4) | (c3 >> 2);
		*oit++ = (c3 << 6) | (c4);
	}

	u8* pad_data = (u8*)&src[pass4_count];
	if (pad == 2) {
		u8 c1 = b64_dec_tab[pad_data[0]],
		   c2 = b64_dec_tab[pad_data[1]];

		*oit++ = (c1 << 2) | (c2 >> 4);
	}
	else if (pad == 1) {
		u8 c1 = b64_dec_tab[pad_data[0]],
		   c2 = b64_dec_tab[pad_data[1]],
		   c3 = b64_dec_tab[pad_data[2]];

		*oit++ = (c1 << 2) | (c2 >> 4);
		*oit++ = (c2 << 4) | (c3 >> 2);
	}

	return oit - dst;
}

