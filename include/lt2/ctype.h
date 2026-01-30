#pragma once

INLINE
int isupper(int c) {
	return c >= 'A' && c <= 'Z';
}

INLINE
int islower(int c) {
	return c >= 'a' && c <= 'z';
}

INLINE
int isalpha(int c) {
	c &= ~32;
	return c >= 'A' && c <= 'Z';
}

INLINE
int isdigit(int c) {
	return c >= '0' && c <= '9';
}

INLINE
int isalnum(int c) {
	return isdigit(c) || isalpha(c);
}

INLINE
int isspace(int c) {
	return (u32)c <= (u8)' ';
}

INLINE
int toupper(int c) {
	if (isupper(c))
		return c - ('a' - 'A');
	return c;
}

INLINE
int tolower(int c) {
	if (isupper(c))
		return c + ('a' - 'A');
	return c;
}

