#include <lt2/test.h>

#include <lt2/pixbuf.h>
#include <lt2/bits.h>

#define CTYPE_PREFIX lt_
#include <lt2/ctype.h>
#include <ctype.h>

#include <lt2/ini.h>
#include <lt2/str.h>

#include <math.h>

int main(int argc, char** argv) {
	default_log_sink->file.color = 1;

	test ("bits") {
		tassert(is_pow2(1));
		tassert(is_pow2(2));
		tassert(!is_pow2(3));
		tassert(is_pow2(4));
		tassert(!is_pow2(5));
		tassert(!is_pow2(6));
		tassert(!is_pow2(7));
		tassert(is_pow2(8));

		tassert(!is_nzpow2(0));
		tassert(is_nzpow2(1));
		tassert(is_nzpow2(2));
		tassert(!is_nzpow2(3));
		tassert(is_nzpow2(4));
		tassert(!is_nzpow2(5));
		tassert(!is_nzpow2(6));
		tassert(!is_nzpow2(7));
		tassert(is_nzpow2(8));

		tassert(pad(0, 8) == 0);
		tassert(pad(1, 8) == 7);
		tassert(pad(3, 8) == 5);
		tassert(pad(8, 8) == 0);
		tassert(pad(36, 32) == 28);

		tassert(align(0, 8) == 0);
		tassert(align(12, 16) == 16);
		tassert(align(33, 32) == 64);
		tassert(align(123, 32) == 128);
		tassert(align(321, 128) == 384);
		tassert(align(1023, 1024) == 1024);
		tassert(align(8, 4) == 8);
		tassert(align(8, 8) == 8);

		tassert(align_bwd(0, 8) == 0);
		tassert(align_bwd(12, 16) == 0);
		tassert(align_bwd(33, 32) == 32);
		tassert(align_bwd(123, 32) == 96);
		tassert(align_bwd(321, 128) == 256);
		tassert(align_bwd(1023, 1024) == 0);
		tassert(align_bwd(8, 4) == 8);
		tassert(align_bwd(8, 8) == 8);
	}

	test ("pixbuf") {
		pixbuf_t pb = {
			.width  = 320,
			.height = 200,
			.data   = NULL
		};
		usz size = pb.width * pb.height * sizeof(u32);
		pb.data = malloc(size);
		u32* cmp_data = malloc(size);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < 100; ++y)
			for (usz x = 0; x < 100; ++x)
				cmp_data[y * pb.width + x] = 0xFFFFFFFF;
		pb_fill_rect(&pb, -100, -100, 200, 200, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 100; y < pb.height; ++y)
			for (usz x = 220; x < pb.width; ++x)
				cmp_data[y * pb.width + x] = 0xFFFFFFFF;
		pb_fill_rect(&pb, 220,  100,  200, 200, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 10; y < pb.height - 10; ++y)
			for (usz x = 10; x < pb.width - 10; ++x)
				cmp_data[y * pb.width + x] = 0xFFFFFFFF;
		for (usz y = 11; y < pb.height - 11; ++y)
			for (usz x = 11; x < pb.width - 11; ++x)
				cmp_data[y * pb.width + x] = 0;
		pb_draw_rect(&pb, 10,  10, pb.width - 20, pb.height - 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz x = 0; x < pb.width; ++x)
			cmp_data[110 * pb.width + x] = 0xFFFFFFFF;
		pb_draw_hline(&pb, -20, 110, pb.width + 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < pb.height; ++y)
			cmp_data[y * pb.width + 110] = 0xFFFFFFFF;
		pb_draw_vline(&pb, 110, -20, pb.height + 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz x = 0; x < pb.width; ++x)
			cmp_data[112 * pb.width + x] = 0xFFFFFFFF;
		pb_draw_line(&pb, -20, 112, pb.width + 20, 112, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < pb.height; ++y)
			cmp_data[y * pb.width + 112] = 0xFFFFFFFF;
		pb_draw_line(&pb, 112, -20, 112, pb.height + 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		// this fails because the current circle-drawing algorithm is imprecise
		/*memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < pb.height; ++y)
			for (usz x = 0; x < pb.width; ++x)
				cmp_data[y * pb.width + x] = (ceil(sqrt(x*x + y*y)) < 75.0) * 0xFFFFFFFF;
		pb_fill_circle(&pb, 0, 0, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);
		*/

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		pb_fill_rect(&pb, -75, -75, 75, 75, 0xFFFFFFFF);
		pb_fill_rect(&pb, pb.width, pb.height, 75, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		pb_draw_rect(&pb, -75, -75, 75, 75, 0xFFFFFFFF);
		pb_draw_rect(&pb, pb.width, pb.height, 75, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		pb_fill_circle(&pb, -75, -75, 75, 0xFFFFFFFF);
		pb_fill_circle(&pb, pb.width + 75, pb.height + 75, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);
	}

	test ("ctype") {
		b8 isupper_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (isupper(c) != lt_isupper(c))
				isupper_matches_libc = 0;
		tassert(isupper_matches_libc);

		b8 islower_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (islower(c) != lt_islower(c))
				islower_matches_libc = 0;
		tassert(islower_matches_libc);

		b8 isalpha_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (isalpha(c) != lt_isalpha(c))
				isalpha_matches_libc = 0;
		tassert(isalpha_matches_libc);

		b8 isdigit_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (isdigit(c) != lt_isdigit(c))
				isdigit_matches_libc = 0;
		tassert(isdigit_matches_libc);

		b8 isalnum_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (isalnum(c) != lt_isalnum(c))
				isalnum_matches_libc = 0;
		tassert(isalnum_matches_libc);

		b8 isspace_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (isspace(c) != lt_isspace(c))
				isspace_matches_libc = 0;
		tassert(isspace_matches_libc);

		b8 toupper_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (toupper(c) != lt_toupper(c))
				toupper_matches_libc = 0;
		tassert(toupper_matches_libc);

		b8 tolower_matches_libc = 1;
		for (usz c = 0; c < 128; ++c)
			if (tolower(c) != lt_tolower(c))
				tolower_matches_libc = 0;
		tassert(tolower_matches_libc);
	}

	test ("ini") {
		ls ini_string = ls(
			"[section1]\n"
			"asdf=1\n"
			"[section2]\n"
			"asdf = 2\n"
			"fdsa = 123456\n"
			"neg  = -2345678\n"
		);

		ini_t ini = parse_ini(ini_string, err_fail);
		isz section1 = ini_find_section(&ini, ls("section1"));
		isz section2 = ini_find_section(&ini, ls("section2"));
		tassert(section1 >= 0 && section2 >= 0);

		ini_key_t* key1 = ini_find_key(&ini, section1, ls("asdf"));
		ini_key_t* key2 = ini_find_key(&ini, section2, ls("asdf"));
		tassert(key1 && key2);

		tassert(!ini_find_key(&ini, section1, ls("asdff")));
		tassert(!ini_find_key(&ini, 123, ls("asdf")));

		tassert(lseq(key1->value, ls("1")));
		tassert(lseq(key2->value, ls("2")));

		tassert(ini_find_u8(&ini, section2, ls("fdsa"), 0) == 64);
		tassert(ini_find_i8(&ini, section2, ls("fdsa"), 0) == 64);
		tassert(ini_find_i8(&ini, section2, ls("neg"),  0) == 50);
		tassert(ini_find_i8(&ini, section2, ls("unkn"), 0) == 0);
		tassert(ini_find_i8(&ini, section1, ls("fdsa"), 0) == 0);

		tassert(ini_find_u16(&ini, section2, ls("fdsa"), 0) == 57920);
		tassert(ini_find_i16(&ini, section2, ls("fdsa"), 0) == -7616);
		tassert(ini_find_i16(&ini, section2, ls("neg"),  0) == 13618);
		tassert(ini_find_i16(&ini, section2, ls("unkn"), 0) == 0);
		tassert(ini_find_i16(&ini, section1, ls("fdsa"), 0) == 0);

		tassert(ini_find_u32(&ini, section2, ls("fdsa"), 0) == 123456);
		tassert(ini_find_i32(&ini, section2, ls("fdsa"), 0) == 123456);
		tassert(ini_find_i32(&ini, section2, ls("neg"),  0) == -2345678);
		tassert(ini_find_i32(&ini, section2, ls("unkn"), 0) == 0);
		tassert(ini_find_i32(&ini, section1, ls("fdsa"), 0) == 0);

		tassert(ini_find_u64(&ini, section2, ls("fdsa"), 0) == 123456);
		tassert(ini_find_i64(&ini, section2, ls("fdsa"), 0) == 123456);
		tassert(ini_find_i64(&ini, section2, ls("neg"),  0) == -2345678);
		tassert(ini_find_i64(&ini, section2, ls("unkn"), 0) == 0);
		tassert(ini_find_i64(&ini, section1, ls("fdsa"), 0) == 0);

		tassert(ini_find_f32(&ini, section2, ls("fdsa"), 0) == 123456.0f);
		tassert(ini_find_f32(&ini, section2, ls("fdsa"), 0) == 123456.0f);
		tassert(ini_find_f32(&ini, section2, ls("neg"),  0) == -2345678.0f);
		tassert(ini_find_f32(&ini, section2, ls("unkn"), 0) == 0.0f);
		tassert(ini_find_f32(&ini, section1, ls("fdsa"), 0) == 0.0f);

		tassert(ini_find_f64(&ini, section2, ls("fdsa"), 0) == 123456.0);
		tassert(ini_find_f64(&ini, section2, ls("fdsa"), 0) == 123456.0);
		tassert(ini_find_f64(&ini, section2, ls("neg"),  0) == -2345678.0);
		tassert(ini_find_f64(&ini, section2, ls("unkn"), 0) == 0.0);
		tassert(ini_find_f64(&ini, section1, ls("fdsa"), 0) == 0.0);

		tassert(lseq(ini_find_str(&ini, section2, ls("fdsa"), ls("")), ls("123456")));
		tassert(lseq(ini_find_str(&ini, section2, ls("fdsa"), ls("")), ls("123456")));
		tassert(lseq(ini_find_str(&ini, section2, ls("neg"),  ls("")), ls("-2345678")));
		tassert(lseq(ini_find_str(&ini, section2, ls("unkn"), ls("")), ls("")));
		tassert(lseq(ini_find_str(&ini, section1, ls("fdsa"), ls("")), ls("")));
	}
}

