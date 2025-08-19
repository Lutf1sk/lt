#include <lt2/page.h>
#include <lt2/str.h>

usz echo(html_template* t, ls str) {
	usz remain = t->end - t->it;
	usz len = str.size;
	if (len > remain)
		len = remain;
	memcpy(t->it, str.ptr, len);
	t->it += len;
	return 0;
}

usz echo_escaped(html_template* t, ls str) {
	for (u8* it = str.ptr, *end = it + str.size; it < end; ++it) {
		char c = *it;
		if (c == '&') {
			if (t->it + 5 > t->end)
				return 0;
			memcpy(t->it, "&amp;", 5);
			t->it += 5;
		}
		else if (c == '<') {
			if (t->it + 4 > t->end)
				return 0;
			memcpy(t->it, "&lt;", 4);
			t->it += 4;
		}
		else if (c == '>') {
			if (t->it + 4 > t->end)
				return 0;
			memcpy(t->it, "&gt;", 4);
			t->it += 4;
		}
		else if (c == '\'') {
			if (t->it + 6 > t->end)
				return 0;
			memcpy(t->it, "&apos;", 6);
			t->it += 6;
		}
		else if (c == '"') {
			if (t->it + 6 > t->end)
				return 0;
			memcpy(t->it, "&quot;", 6);
			t->it += 6;
		}
		else {
			if (t->it >= t->end)
				return 0;
			*t->it++ = c;
		}
	}
	return 0;
}

usz echo_attributes(html_template* t, char* fmt, ...) {
	va_list argl;
	va_start(argl, fmt);

	for (char* it = fmt; *it; ++it) {
		char c = *it;
		if (c != '{') {
			*t->it++ = c;
			continue;
		}
		if (it[1] == '{') {
			*t->it++ = *++it;
			continue;
		}

		char* type_start = ++it;
		while (*it && *it != '}')
			++it;
		ls type = lsrange(type_start, it);

		if (lseq(type, ls("ls")))
			echo_escaped(t, va_arg(argl, ls));
		else if (lseq(type, ls("char*")))
			echo_escaped(t, stols(va_arg(argl, char*)));
	}

	va_end(argl);
	return 0;
}

