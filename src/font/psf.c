#include <lt/font.h>
#include <lt/mem.h>
#include <lt/io.h>
#include <lt/internal.h>

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF1_SEPARATOR  0xFFFF
#define PSF1_STARTSEQ   0xFFFE

typedef
struct lt_psf1_header {
	u16 magic;
	u8 mode;
	u8 height;
} lt_psf1_header_t;

#define PSF2_UNICODE_TAB 0x01

#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE

typedef
struct lt_psf2_header {
	u32 magic;
	u32 version;
	u32 header_size;
	u32 flags;
	u32 glyph_count;
	u32 glyph_bytes;
	u32 height;
	u32 width;
} lt_psf2_header_t;

#define _ 0x00000000,
#define X 0xFFFFFFFF,

static
u32 precomp_tab[256][8] = {
	[0b00000000] = { _ _ _ _ _ _ _ _ },
	[0b00000001] = { _ _ _ _ _ _ _ X },
	[0b00000010] = { _ _ _ _ _ _ X _ },
	[0b00000011] = { _ _ _ _ _ _ X X },
	[0b00000100] = { _ _ _ _ _ X _ _ },
	[0b00000101] = { _ _ _ _ _ X _ X },
	[0b00000110] = { _ _ _ _ _ X X _ },
	[0b00000111] = { _ _ _ _ _ X X X },
	[0b00001000] = { _ _ _ _ X _ _ _ },
	[0b00001001] = { _ _ _ _ X _ _ X },
	[0b00001010] = { _ _ _ _ X _ X _ },
	[0b00001011] = { _ _ _ _ X _ X X },
	[0b00001100] = { _ _ _ _ X X _ _ },
	[0b00001101] = { _ _ _ _ X X _ X },
	[0b00001110] = { _ _ _ _ X X X _ },
	[0b00001111] = { _ _ _ _ X X X X },
	[0b00010000] = { _ _ _ X _ _ _ _ },
	[0b00010001] = { _ _ _ X _ _ _ X },
	[0b00010010] = { _ _ _ X _ _ X _ },
	[0b00010011] = { _ _ _ X _ _ X X },
	[0b00010100] = { _ _ _ X _ X _ _ },
	[0b00010101] = { _ _ _ X _ X _ X },
	[0b00010110] = { _ _ _ X _ X X _ },
	[0b00010111] = { _ _ _ X _ X X X },
	[0b00011000] = { _ _ _ X X _ _ _ },
	[0b00011001] = { _ _ _ X X _ _ X },
	[0b00011010] = { _ _ _ X X _ X _ },
	[0b00011011] = { _ _ _ X X _ X X },
	[0b00011100] = { _ _ _ X X X _ _ },
	[0b00011101] = { _ _ _ X X X _ X },
	[0b00011110] = { _ _ _ X X X X _ },
	[0b00011111] = { _ _ _ X X X X X },
	[0b00100000] = { _ _ X _ _ _ _ _ },
	[0b00100001] = { _ _ X _ _ _ _ X },
	[0b00100010] = { _ _ X _ _ _ X _ },
	[0b00100011] = { _ _ X _ _ _ X X },
	[0b00100100] = { _ _ X _ _ X _ _ },
	[0b00100101] = { _ _ X _ _ X _ X },
	[0b00100110] = { _ _ X _ _ X X _ },
	[0b00100111] = { _ _ X _ _ X X X },
	[0b00101000] = { _ _ X _ X _ _ _ },
	[0b00101001] = { _ _ X _ X _ _ X },
	[0b00101010] = { _ _ X _ X _ X _ },
	[0b00101011] = { _ _ X _ X _ X X },
	[0b00101100] = { _ _ X _ X X _ _ },
	[0b00101101] = { _ _ X _ X X _ X },
	[0b00101110] = { _ _ X _ X X X _ },
	[0b00101111] = { _ _ X _ X X X X },
	[0b00110000] = { _ _ X X _ _ _ _ },
	[0b00110001] = { _ _ X X _ _ _ X },
	[0b00110010] = { _ _ X X _ _ X _ },
	[0b00110011] = { _ _ X X _ _ X X },
	[0b00110100] = { _ _ X X _ X _ _ },
	[0b00110101] = { _ _ X X _ X _ X },
	[0b00110110] = { _ _ X X _ X X _ },
	[0b00110111] = { _ _ X X _ X X X },
	[0b00111000] = { _ _ X X X _ _ _ },
	[0b00111001] = { _ _ X X X _ _ X },
	[0b00111010] = { _ _ X X X _ X _ },
	[0b00111011] = { _ _ X X X _ X X },
	[0b00111100] = { _ _ X X X X _ _ },
	[0b00111101] = { _ _ X X X X _ X },
	[0b00111110] = { _ _ X X X X X _ },
	[0b00111111] = { _ _ X X X X X X },
	[0b01000000] = { _ X _ _ _ _ _ _ },
	[0b01000001] = { _ X _ _ _ _ _ X },
	[0b01000010] = { _ X _ _ _ _ X _ },
	[0b01000011] = { _ X _ _ _ _ X X },
	[0b01000100] = { _ X _ _ _ X _ _ },
	[0b01000101] = { _ X _ _ _ X _ X },
	[0b01000110] = { _ X _ _ _ X X _ },
	[0b01000111] = { _ X _ _ _ X X X },
	[0b01001000] = { _ X _ _ X _ _ _ },
	[0b01001001] = { _ X _ _ X _ _ X },
	[0b01001010] = { _ X _ _ X _ X _ },
	[0b01001011] = { _ X _ _ X _ X X },
	[0b01001100] = { _ X _ _ X X _ _ },
	[0b01001101] = { _ X _ _ X X _ X },
	[0b01001110] = { _ X _ _ X X X _ },
	[0b01001111] = { _ X _ _ X X X X },
	[0b01010000] = { _ X _ X _ _ _ _ },
	[0b01010001] = { _ X _ X _ _ _ X },
	[0b01010010] = { _ X _ X _ _ X _ },
	[0b01010011] = { _ X _ X _ _ X X },
	[0b01010100] = { _ X _ X _ X _ _ },
	[0b01010101] = { _ X _ X _ X _ X },
	[0b01010110] = { _ X _ X _ X X _ },
	[0b01010111] = { _ X _ X _ X X X },
	[0b01011000] = { _ X _ X X _ _ _ },
	[0b01011001] = { _ X _ X X _ _ X },
	[0b01011010] = { _ X _ X X _ X _ },
	[0b01011011] = { _ X _ X X _ X X },
	[0b01011100] = { _ X _ X X X _ _ },
	[0b01011101] = { _ X _ X X X _ X },
	[0b01011110] = { _ X _ X X X X _ },
	[0b01011111] = { _ X _ X X X X X },
	[0b01100000] = { _ X X _ _ _ _ _ },
	[0b01100001] = { _ X X _ _ _ _ X },
	[0b01100010] = { _ X X _ _ _ X _ },
	[0b01100011] = { _ X X _ _ _ X X },
	[0b01100100] = { _ X X _ _ X _ _ },
	[0b01100101] = { _ X X _ _ X _ X },
	[0b01100110] = { _ X X _ _ X X _ },
	[0b01100111] = { _ X X _ _ X X X },
	[0b01101000] = { _ X X _ X _ _ _ },
	[0b01101001] = { _ X X _ X _ _ X },
	[0b01101010] = { _ X X _ X _ X _ },
	[0b01101011] = { _ X X _ X _ X X },
	[0b01101100] = { _ X X _ X X _ _ },
	[0b01101101] = { _ X X _ X X _ X },
	[0b01101110] = { _ X X _ X X X _ },
	[0b01101111] = { _ X X _ X X X X },
	[0b01110000] = { _ X X X _ _ _ _ },
	[0b01110001] = { _ X X X _ _ _ X },
	[0b01110010] = { _ X X X _ _ X _ },
	[0b01110011] = { _ X X X _ _ X X },
	[0b01110100] = { _ X X X _ X _ _ },
	[0b01110101] = { _ X X X _ X _ X },
	[0b01110110] = { _ X X X _ X X _ },
	[0b01110111] = { _ X X X _ X X X },
	[0b01111000] = { _ X X X X _ _ _ },
	[0b01111001] = { _ X X X X _ _ X },
	[0b01111010] = { _ X X X X _ X _ },
	[0b01111011] = { _ X X X X _ X X },
	[0b01111100] = { _ X X X X X _ _ },
	[0b01111101] = { _ X X X X X _ X },
	[0b01111110] = { _ X X X X X X _ },
	[0b01111111] = { _ X X X X X X X },
	[0b10000000] = { X _ _ _ _ _ _ _ },
	[0b10000001] = { X _ _ _ _ _ _ X },
	[0b10000010] = { X _ _ _ _ _ X _ },
	[0b10000011] = { X _ _ _ _ _ X X },
	[0b10000100] = { X _ _ _ _ X _ _ },
	[0b10000101] = { X _ _ _ _ X _ X },
	[0b10000110] = { X _ _ _ _ X X _ },
	[0b10000111] = { X _ _ _ _ X X X },
	[0b10001000] = { X _ _ _ X _ _ _ },
	[0b10001001] = { X _ _ _ X _ _ X },
	[0b10001010] = { X _ _ _ X _ X _ },
	[0b10001011] = { X _ _ _ X _ X X },
	[0b10001100] = { X _ _ _ X X _ _ },
	[0b10001101] = { X _ _ _ X X _ X },
	[0b10001110] = { X _ _ _ X X X _ },
	[0b10001111] = { X _ _ _ X X X X },
	[0b10010000] = { X _ _ X _ _ _ _ },
	[0b10010001] = { X _ _ X _ _ _ X },
	[0b10010010] = { X _ _ X _ _ X _ },
	[0b10010011] = { X _ _ X _ _ X X },
	[0b10010100] = { X _ _ X _ X _ _ },
	[0b10010101] = { X _ _ X _ X _ X },
	[0b10010110] = { X _ _ X _ X X _ },
	[0b10010111] = { X _ _ X _ X X X },
	[0b10011000] = { X _ _ X X _ _ _ },
	[0b10011001] = { X _ _ X X _ _ X },
	[0b10011010] = { X _ _ X X _ X _ },
	[0b10011011] = { X _ _ X X _ X X },
	[0b10011100] = { X _ _ X X X _ _ },
	[0b10011101] = { X _ _ X X X _ X },
	[0b10011110] = { X _ _ X X X X _ },
	[0b10011111] = { X _ _ X X X X X },
	[0b10100000] = { X _ X _ _ _ _ _ },
	[0b10100001] = { X _ X _ _ _ _ X },
	[0b10100010] = { X _ X _ _ _ X _ },
	[0b10100011] = { X _ X _ _ _ X X },
	[0b10100100] = { X _ X _ _ X _ _ },
	[0b10100101] = { X _ X _ _ X _ X },
	[0b10100110] = { X _ X _ _ X X _ },
	[0b10100111] = { X _ X _ _ X X X },
	[0b10101000] = { X _ X _ X _ _ _ },
	[0b10101001] = { X _ X _ X _ _ X },
	[0b10101010] = { X _ X _ X _ X _ },
	[0b10101011] = { X _ X _ X _ X X },
	[0b10101100] = { X _ X _ X X _ _ },
	[0b10101101] = { X _ X _ X X _ X },
	[0b10101110] = { X _ X _ X X X _ },
	[0b10101111] = { X _ X _ X X X X },
	[0b10110000] = { X _ X X _ _ _ _ },
	[0b10110001] = { X _ X X _ _ _ X },
	[0b10110010] = { X _ X X _ _ X _ },
	[0b10110011] = { X _ X X _ _ X X },
	[0b10110100] = { X _ X X _ X _ _ },
	[0b10110101] = { X _ X X _ X _ X },
	[0b10110110] = { X _ X X _ X X _ },
	[0b10110111] = { X _ X X _ X X X },
	[0b10111000] = { X _ X X X _ _ _ },
	[0b10111001] = { X _ X X X _ _ X },
	[0b10111010] = { X _ X X X _ X _ },
	[0b10111011] = { X _ X X X _ X X },
	[0b10111100] = { X _ X X X X _ _ },
	[0b10111101] = { X _ X X X X _ X },
	[0b10111110] = { X _ X X X X X _ },
	[0b10111111] = { X _ X X X X X X },
	[0b11000000] = { X X _ _ _ _ _ _ },
	[0b11000001] = { X X _ _ _ _ _ X },
	[0b11000010] = { X X _ _ _ _ X _ },
	[0b11000011] = { X X _ _ _ _ X X },
	[0b11000100] = { X X _ _ _ X _ _ },
	[0b11000101] = { X X _ _ _ X _ X },
	[0b11000110] = { X X _ _ _ X X _ },
	[0b11000111] = { X X _ _ _ X X X },
	[0b11001000] = { X X _ _ X _ _ _ },
	[0b11001001] = { X X _ _ X _ _ X },
	[0b11001010] = { X X _ _ X _ X _ },
	[0b11001011] = { X X _ _ X _ X X },
	[0b11001100] = { X X _ _ X X _ _ },
	[0b11001101] = { X X _ _ X X _ X },
	[0b11001110] = { X X _ _ X X X _ },
	[0b11001111] = { X X _ _ X X X X },
	[0b11010000] = { X X _ X _ _ _ _ },
	[0b11010001] = { X X _ X _ _ _ X },
	[0b11010010] = { X X _ X _ _ X _ },
	[0b11010011] = { X X _ X _ _ X X },
	[0b11010100] = { X X _ X _ X _ _ },
	[0b11010101] = { X X _ X _ X _ X },
	[0b11010110] = { X X _ X _ X X _ },
	[0b11010111] = { X X _ X _ X X X },
	[0b11011000] = { X X _ X X _ _ _ },
	[0b11011001] = { X X _ X X _ _ X },
	[0b11011010] = { X X _ X X _ X _ },
	[0b11011011] = { X X _ X X _ X X },
	[0b11011100] = { X X _ X X X _ _ },
	[0b11011101] = { X X _ X X X _ X },
	[0b11011110] = { X X _ X X X X _ },
	[0b11011111] = { X X _ X X X X X },
	[0b11100000] = { X X X _ _ _ _ _ },
	[0b11100001] = { X X X _ _ _ _ X },
	[0b11100010] = { X X X _ _ _ X _ },
	[0b11100011] = { X X X _ _ _ X X },
	[0b11100100] = { X X X _ _ X _ _ },
	[0b11100101] = { X X X _ _ X _ X },
	[0b11100110] = { X X X _ _ X X _ },
	[0b11100111] = { X X X _ _ X X X },
	[0b11101000] = { X X X _ X _ _ _ },
	[0b11101001] = { X X X _ X _ _ X },
	[0b11101010] = { X X X _ X _ X _ },
	[0b11101011] = { X X X _ X _ X X },
	[0b11101100] = { X X X _ X X _ _ },
	[0b11101101] = { X X X _ X X _ X },
	[0b11101110] = { X X X _ X X X _ },
	[0b11101111] = { X X X _ X X X X },
	[0b11110000] = { X X X X _ _ _ _ },
	[0b11110001] = { X X X X _ _ _ X },
	[0b11110010] = { X X X X _ _ X _ },
	[0b11110011] = { X X X X _ _ X X },
	[0b11110100] = { X X X X _ X _ _ },
	[0b11110101] = { X X X X _ X _ X },
	[0b11110110] = { X X X X _ X X _ },
	[0b11110111] = { X X X X _ X X X },
	[0b11111000] = { X X X X X _ _ _ },
	[0b11111001] = { X X X X X _ _ X },
	[0b11111010] = { X X X X X _ X _ },
	[0b11111011] = { X X X X X _ X X },
	[0b11111100] = { X X X X X X _ _ },
	[0b11111101] = { X X X X X X _ X },
	[0b11111110] = { X X X X X X X _ },
	[0b11111111] = { X X X X X X X X },
};

#undef _
#undef X

static
lt_err_t read_header1(void* data, usz len, lt_font_t out_font[static 1], u8* out_glyph_start[static 1]) {
	lt_psf1_header_t* head = data;

	if (len < sizeof(lt_psf1_header_t) || head->height == 0)
		return LT_ERR_INVALID_FORMAT;

	out_font->width = 8;
	out_font->height = head->height;
	out_font->glyph_count = 256;
	if (head->mode & PSF1_MODE512)
		out_font->glyph_count = 512;

	*out_glyph_start = (u8*)data + sizeof(lt_psf1_header_t);
	return LT_SUCCESS;
}

static
lt_err_t read_header2(void* data, usz len, lt_font_t out_font[static 1], u8* out_glyph_start[static 1]) {
	lt_psf2_header_t* head = data;

	if (len < sizeof(lt_psf2_header_t) || head->version != 0)
		return LT_ERR_INVALID_FORMAT;

	usz glyphtab_size = head->glyph_count * head->glyph_bytes;
	if (len < head->header_size + glyphtab_size || head->width == 0 || head->height == 0)
		return LT_ERR_INVALID_FORMAT;

	out_font->width = head->width;
	out_font->height = head->height;
	out_font->glyph_count = head->glyph_count;

	*out_glyph_start = (u8*)data + head->header_size;
	return LT_SUCCESS;
}

lt_err_t lt_font_load_psf(lt_font_t font[static 1], void* data, usz len, lt_alloc_t alloc[static 1]) {
	lt_err_t err;

	if (len < sizeof(u32))
		return LT_ERR_INVALID_FORMAT;

	// Read PSF header
	u8* it = NULL;
	if (*(u16*)data == LT_FONT_PSF1_MAGIC && (err = read_header1(data, len, font, &it)))
		return err;
	if (*(u32*)data == LT_FONT_PSF2_MAGIC && (err = read_header2(data, len, font, &it)))
		return err;
	if (!it)
		return LT_ERR_INVALID_FORMAT;

	// Allocate target buffer
	font->glyph_data = lt_malloc(alloc, font->glyph_count * font->height * font->width * sizeof(u32));
	if (!font->glyph_data)
		return LT_ERR_OUT_OF_MEMORY;

	// Convert to RGBA image
	u32* out = font->glyph_data;
	usz row_count = font->glyph_count * font->height;
	for (usz i = 0; i < row_count; ++i) {
		i32 bits = font->width;

		while (bits > 0) {
			usz left = bits < 8 ? 8 - bits : 8;

			memcpy(out, precomp_tab[*it++], left * sizeof(u32));
			out += left;
			bits -= left;
		}
	}

	return LT_SUCCESS;
}

lt_err_t lt_font_write_psf(const lt_font_t font[static 1], lt_file_t* file, lt_alloc_t alloc[static 1]) {
	lt_err_t err;

	// Allocate target buffer
	usz w_bytes = font->width / 8;
	if (font->width % 8)
		++w_bytes;
	usz glyph_bytes = w_bytes * font->height;
	usz glyphtab_size = glyph_bytes * font->glyph_count;
	u8* glyphtab = lt_malloc(alloc, glyphtab_size);
	if (!glyphtab)
		fail_to(err = LT_ERR_OUT_OF_MEMORY, err0);

	// Convert to bitmap
	u8* out_it = glyphtab;
	u32* in_it = font->glyph_data;
	usz row_count = font->glyph_count * font->height;
	for (usz i = 0; i < row_count; ++i) {
		u8 byte = 0;
		u8 mask = 7;

		for (usz i = 0; i < font->width; ++i) {
			u8 i_masked = i & mask;

			if (i && !i_masked) {
				*out_it++ = byte;
				byte = 0;
			}

			u8 bit = !!(*in_it++ & 0xFF);
			u8 shift = mask - i_masked;
			byte |= bit << shift;
		}

		*out_it++ = byte;
	}

	// Fill PSF header
	lt_psf2_header_t head;
	head.magic = LT_FONT_PSF2_MAGIC;
	head.version = 0;
	head.header_size = sizeof(lt_psf2_header_t);
	head.flags = 0;
	head.glyph_bytes = glyph_bytes;
	head.glyph_count = font->glyph_count;
	head.height = font->height;
	head.width = font->width;

	// Write to file
	if (lt_fwrite(file, &head, sizeof(head)) != sizeof(head))
		fail_to(err = LT_ERR_UNKNOWN, err1); // !!
	if (lt_fwrite(file, glyphtab, glyphtab_size) != glyphtab_size)
		fail_to(err = LT_ERR_UNKNOWN, err1); // !!

	err = LT_SUCCESS;

err1:	lt_mfree(alloc, glyphtab);
err0:	return err;
}
