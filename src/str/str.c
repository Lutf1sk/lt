#include <lt/str.h>

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


