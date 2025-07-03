#pragma once

#include <lt2/common.h>

#define LABEL_ADDR(label) (&&label)
#define GOTO_ADDR(addr) goto *(addr)

#define CAT2(a, b) a ## b
#define CAT(a, b) CAT2(a, b)
#define UNIQUE(n) CAT(CAT(__lt_unique_, __LINE__), _##n)

#define enter_task() \
	if (!__async->reenter_at)\
		__async->reenter_at = LABEL_ADDR(__body); \
	void* __jump_addr = __async->reenter_at; \
	__async->reenter_at = NULL; \
	GOTO_ADDR(__jump_addr); \
__body: 

#define async task __async[static 1]

#define yield \
	for (__async->reenter_at = LABEL_ADDR(UNIQUE(0)); __async->reenter_at;) \
	UNIQUE(0): for (; __async->reenter_at;) \
			return

#define await(t, func) \
	UNIQUE(0): for (; ((func), (t)->running && (__async->reenter_at = LABEL_ADDR(UNIQUE(0))));) \
		return

#define awaitv(out, t, func) \
	UNIQUE(0): for (; (((out) = (func)), (t)->running && (__async->reenter_at = LABEL_ADDR(UNIQUE(0))));) \
		return

// !! this really should have a short sleep instead of immediately rerunning
#define invoke(t, func) \
	do { \
		func; \
	} while ((t)->running)

