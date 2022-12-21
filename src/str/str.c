#include <lt/str.h>
#include <lt/ctype.h>

b8 lt_lstr_case_eq(lstr_t s1, lstr_t s2) {
	if (s1.len != s2.len)
		return 0;
	for (usz i = 0; i < s1.len; ++i)
		if (lt_to_upper(s1.str[i]) != lt_to_upper(s2.str[i]))
			return 0;
	return 1;
}

lstr_t lt_lstr_trim_left(lstr_t str) {
	char *it, *end;
	for (it = str.str, end = it + str.len; it < end && lt_is_space(*it); ++it)
		;
	return LSTR(it, end - it);
}

lstr_t lt_lstr_trim_right(lstr_t str) {
	char *it;
	for (it = str.str + str.len; it > str.str && lt_is_space(*(it - 1)); --it)
		;
	return LSTR(str.str, it - str.str);
}

LT_FLATTEN
lstr_t lt_lstr_trim(lstr_t str) {
	return lt_lstr_trim_left(lt_lstr_trim_right(str));
}

f64 lt_lstr_float(lstr_t str) {
	if (!str.len)
		return 0;

	usz it = 0;

	b8 sign = str.str[0] == '-';
	if (sign)
		++it;

	u64 val = 0;

	f64 float_val = 0;

	while (it < str.len) {
		if (str.str[it] == '.') {
			++it;
			u64 decimal = 0;
			u64 decimal_mult = 1;
			while (it < str.len) {
				decimal_mult *= 10;
				decimal *= 10;
				decimal += str.str[it++] - '0';
			}
			float_val = ((f64)val + (f64)decimal / (f64)decimal_mult);
			goto done;
		}
		val *= 10;
		val += str.str[it++] - '0';
	}
	float_val = (f64)val;
done:
	if (sign)
		float_val = -float_val;
	return float_val;
}

i64 lt_lstr_int(lstr_t str) {
	if (!str.len)
		return 0;

	b8 sign = str.str[0] == '-';
	if (sign) {
		++str.str;
		--str.len;
		return -lt_lstr_uint(str);
	}

	return lt_lstr_uint(str);
}

u64 lt_lstr_uint(lstr_t str) {
	u64 val = 0;

	usz i = 0;
	while (i < str.len) {
		val *= 10;
		val += str.str[i++] - '0';
	}

	return val;
}

static u8 hex_conv_tab[256] = {
	['0'] = 0x0,
	['1'] = 0x1,
	['2'] = 0x2,
	['3'] = 0x3,
	['4'] = 0x4,
	['5'] = 0x5,
	['6'] = 0x6,
	['7'] = 0x7,
	['8'] = 0x8,
	['9'] = 0x9,

	['A'] = 0xA, ['a'] = 0xA,
	['B'] = 0xB, ['b'] = 0xB,
	['C'] = 0xC, ['c'] = 0xC,
	['D'] = 0xD, ['d'] = 0xD,
	['E'] = 0xE, ['e'] = 0xE,
	['F'] = 0xF, ['f'] = 0xF,
 };

u64 lt_lstr_hex_uint(lstr_t str) {
	u64 val = 0;

	usz i = 0;
	while (i < str.len) {
		u8 c = str.str[i++];

		val <<= 4;
		val += hex_conv_tab[c];
	}

	return val;
}

