#include <lt/base64.h>
#include <lt/mem.h>

static u8 base64_enc_tab[64];
static u8 base64_dec_tab[128];

void lt_base64_init(void) {
	memset(base64_enc_tab, 0, sizeof(base64_enc_tab));
	memset(base64_dec_tab, 0, sizeof(base64_dec_tab));

	for (usz i = 0; i < 26; ++i) {
		u8 symbol = i + 'A';
		base64_enc_tab[i] = symbol;
		base64_dec_tab[symbol] = i;
	}

	for (usz i = 0; i < 26; ++i) {
		u8 symbol = i + 'a';
		u8 index = i + 26;
		base64_enc_tab[index] = symbol;
		base64_dec_tab[symbol] = index;
	}

	for (usz i = 0; i < 10; ++i) {
		u8 symbol = i + '0';
		u8 index = i + 52;
		base64_enc_tab[index] = symbol;
		base64_dec_tab[symbol] = index;
	}

	base64_enc_tab[62] = '+';
	base64_enc_tab[63] = '/';

	base64_dec_tab['+'] = 62;
	base64_dec_tab['/'] = 63;
}

usz lt_base64_encode(lstr_t str, char* out) {
	char* oit = out;

	usz pass3_rem = str.len % 3;
	usz pass3_count = str.len - pass3_rem;
	for (usz i = 0; i < pass3_count; i += 3) {
		char* data = &str.str[i];
		char c1 = data[0], c2 = data[1], c3 = data[2];

		*(oit++) = base64_enc_tab[c1 >> 2];
		*(oit++) = base64_enc_tab[((c1 & 0b000011) << 4) | (c2 >> 4)];
		*(oit++) = base64_enc_tab[((c2 & 0b001111) << 2) | (c3 >> 6)];
		*(oit++) = base64_enc_tab[c3 & 0b111111];
	}

	char* rem_data = &str.str[pass3_count];
	if (pass3_rem == 1) {
		char c1 = rem_data[0];
		*(oit++) = base64_enc_tab[c1 >> 2];
		*(oit++) = base64_enc_tab[(c1 & 0b000011) << 4];
	}
	else if (pass3_rem == 2) {
		char c1 = rem_data[0], c2 = rem_data[1];
		*(oit++) = base64_enc_tab[c1 >> 2];
		*(oit++) = base64_enc_tab[((c1 & 0b000011) << 4) | (c2 >> 4)];
		*(oit++) = base64_enc_tab[(c2 & 0b001111) << 2];
	}

	switch (3 - pass3_rem) {
		case 2: *(oit++) = '=';
		case 1: *(oit++) = '=';
		default: break;
	}

	return oit - out;
}

usz lt_base64_decode(lstr_t str, char* out) {
	usz pad = 0;
	while (pad < 3 && str.str[str.len - pad - 1] == '=')
		++pad;

	usz pass4_count = (str.len - pad) & ~0b11;
	char* oit = out;

	for (usz i = 0; i < pass4_count; i += 4) {
		u8* data = (u8*)&str.str[i];
		char c1 = base64_dec_tab[data[0]], c2 = base64_dec_tab[data[1]], c3 = base64_dec_tab[data[2]], c4 = base64_dec_tab[data[3]];

		*(oit++) = (c1 << 2) | (c2 >> 4);
		*(oit++) = (c2 << 4) | (c3 >> 2);
		*(oit++) = (c3 << 6) | (c4);
	}

	u8* pad_data = (u8*)&str.str[pass4_count];
	if (pad == 2) {
		char c1 = base64_dec_tab[pad_data[0]], c2 = base64_dec_tab[pad_data[1]];
		*(oit++) = (c1 << 2) | (c2 >> 4);
	}
	else if (pad == 1) {
		char c1 = base64_dec_tab[pad_data[0]], c2 = base64_dec_tab[pad_data[1]], c3 = base64_dec_tab[pad_data[2]];
		*(oit++) = (c1 << 2) | (c2 >> 4);
		*(oit++) = (c2 << 4) | (c3 >> 2);
	}

	return oit - out;
}

