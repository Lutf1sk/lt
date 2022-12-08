#ifndef LT_INTERNAL_H
#define LT_INTERNAL_H 1

#define fail_to(x, lbl) do { \
	x; \
	goto lbl; \
} while(0)

#endif
