#include <lt/base64.h>
#include <lt/mem.h>

static const u8 base64_enc_tab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const u8 base64_dec_tab[256] = {
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

usz lt_base64_encode(lstr_t str, char* out) {
	char* oit = out;

	usz pass3_rem = str.len % 3;
	usz pass3_count = str.len - pass3_rem;
	for (usz i = 0; i < pass3_count; i += 3) {
		char* data = &str.str[i];
		char c1 = data[0], c2 = data[1], c3 = data[2];

		*oit++ = base64_enc_tab[c1 >> 2];
		*oit++ = base64_enc_tab[((c1 & 0b000011) << 4) | (c2 >> 4)];
		*oit++ = base64_enc_tab[((c2 & 0b001111) << 2) | (c3 >> 6)];
		*oit++ = base64_enc_tab[c3 & 0b111111];
	}

	char* rem_data = &str.str[pass3_count];
	if (pass3_rem == 1) {
		char c1 = rem_data[0];
		*oit++ = base64_enc_tab[c1 >> 2];
		*oit++ = base64_enc_tab[(c1 & 0b000011) << 4];
		*oit++ = '=';
		*oit++ = '=';
	}
	else if (pass3_rem == 2) {
		char c1 = rem_data[0], c2 = rem_data[1];
		*oit++ = base64_enc_tab[c1 >> 2];
		*oit++ = base64_enc_tab[((c1 & 0b000011) << 4) | (c2 >> 4)];
		*oit++ = base64_enc_tab[(c2 & 0b001111) << 2];
		*oit++ = '=';
	}

	return oit - out;
}

usz lt_base64_decode(lstr_t str, char* out) {
	usz pad = 0;
	while (pad < 3 && str.str[str.len - pad - 1] == '=')
		++pad;

	if (pad == 1 || pad > 2)
		return 0;

	usz pass4_count = (str.len - pad) & ~0b11;
	char* oit = out;

	for (usz i = 0; i < pass4_count; i += 4) {
		u8* data = (u8*)&str.str[i];
		char c1 = base64_dec_tab[data[0]], c2 = base64_dec_tab[data[1]], c3 = base64_dec_tab[data[2]], c4 = base64_dec_tab[data[3]];

		*oit++ = (c1 << 2) | (c2 >> 4);
		*oit++ = (c2 << 4) | (c3 >> 2);
		*oit++ = (c3 << 6) | (c4);
	}

	u8* pad_data = (u8*)&str.str[pass4_count];
	if (pad == 2) {
		char c1 = base64_dec_tab[pad_data[0]], c2 = base64_dec_tab[pad_data[1]];
		*oit++ = (c1 << 2) | (c2 >> 4);
	}
	else if (pad == 1) {
		char c1 = base64_dec_tab[pad_data[0]], c2 = base64_dec_tab[pad_data[1]], c3 = base64_dec_tab[pad_data[2]];
		*oit++ = (c1 << 2) | (c2 >> 4);
		*oit++ = (c2 << 4) | (c3 >> 2);
	}

	return oit - out;
}

