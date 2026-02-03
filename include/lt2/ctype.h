#pragma once

#ifndef CTYPE_PREFIX
#	define CTYPE_PREFIX
#endif

INLINE
int EXCAT(CTYPE_PREFIX, isupper)(int c) {
	return c >= 'A' && c <= 'Z';
}

INLINE
int EXCAT(CTYPE_PREFIX, islower)(int c) {
	return c >= 'a' && c <= 'z';
}

INLINE
int EXCAT(CTYPE_PREFIX, isalpha)(int c) {
	c &= ~32;
	return c >= 'A' && c <= 'Z';
}

INLINE
int EXCAT(CTYPE_PREFIX, isdigit)(int c) {
	return c >= '0' && c <= '9';
}

INLINE
int EXCAT(CTYPE_PREFIX, isalnum)(int c) {
	return EXCAT(CTYPE_PREFIX, isdigit)(c) || EXCAT(CTYPE_PREFIX, isalpha)(c);
}

INLINE
int EXCAT(CTYPE_PREFIX, isspace)(int c) {
	if LIKELY (c > 32)
		return 0;

	return
		c == '\t' ||
		c == '\n' ||
		c == '\v' ||
		c == '\f' ||
		c == '\r' ||
		c == ' ';
}

INLINE
int EXCAT(CTYPE_PREFIX, toupper)(int c) {
	if (EXCAT(CTYPE_PREFIX, islower(c)))
		return c - ('a' - 'A');
	return c;
}

INLINE
int EXCAT(CTYPE_PREFIX, tolower)(int c) {
	if (EXCAT(CTYPE_PREFIX, isupper)(c))
		return c + ('a' - 'A');
	return c;
}

