#include <lt/strstream.h>
#include <lt/str.h>
#include <lt/html.h>

b8 lt_html_reserved_char_tab[LT_U8_MAX] = {
	['\''] = 1,
	['"'] = 1,
	['<'] = 1,
	['>'] = 1,
	['&'] = 1,
};

static
lstr_t conv_tab[] = {
	['\''] = CLSTR("&apos;"),
	['"'] = CLSTR("&quot;"),
	['<'] = CLSTR("&lt;"),
	['>'] = CLSTR("&gt;"),
	['&'] = CLSTR("&amp;"),
};

LT_FLATTEN
lstr_t lt_htmlencode(lstr_t str, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_strstream_t ss;
	if ((err = lt_strstream_create(&ss, alloc))) {
		lt_werrf("unhandled %S in lt_htmlencode\n", lt_err_str(err));
		return NLSTR();
	}
	if (lt_write_htmlencoded((lt_io_callback_t)lt_strstream_write, &ss, str) < 0) {
		// !!
	}
	return ss.str;
}

isz lt_write_htmlencoded(lt_io_callback_t callb, void* usr, lstr_t str) {
	isz res;

	usz written = 0;

	char* it = str.str, *end = it + str.len;
	char* start = it;
	while (it < end) {
		u8 c = *it;

		if (!lt_html_reserved_char_tab[c]) {
			++it;
			continue;
		}

		if ((res = lt_writels(callb, usr, lt_lsfrom_range(start, it++))) < 0) {
			return -res;
		}
		start = it;
		written += res;

		if ((res = lt_writels(callb, usr, conv_tab[c])) < 0) {
			return -res;
		}
		written += res;
	}

	if ((res = lt_writels(callb, usr, lt_lsfrom_range(start, it)) < 0)) {
		return -res;
	}

	return written + res;
}

isz lt_write_htmlencoded_char8(lt_io_callback_t callb, void* usr, u8 c) {
	isz res;

	if (!lt_html_reserved_char_tab[c]) {
		if ((res = callb(usr, &c, 1)) < 0) {
			return -res;
		}
		return 1;
	}

	if ((res = lt_writels(callb, usr, conv_tab[c])) < 0) {
		return -res;
	}
	return res;
}
