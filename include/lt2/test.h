#pragma once

#include <lt2/common.h>
#include <lt2/log.h>

typedef struct result {
	const char* name;
	u32 count;
	u32 failed;
} result_t;

INLINE
void after_test(result_t* res) {
	if (res->failed)
		llogf(NULL, LOG_ALERT, "[{char*}] {u32} failed ({u32} total)", res->name, res->failed, res->count);
	else
		llogf(NULL, LOG_INFO, "[{char*}] {u32} passed", res->name, res->failed, res->count);
}

INLINE
void before_tassert(result_t* r) {
	++r->count;
}

static
void fail_tassert(result_t* r, const char* expr) {
	++r->failed;
	llogf(NULL, LOG_ALERT, "[{char*}] {char*}", r->name, expr);
}

#define test(n) \
	for (result_t* __tres = &(result_t) { .name = n }; __tres; after_test(__tres), (__tres = NULL))

#define tassert(e) \
	do { \
		before_tassert(__tres); \
		if (!(e)) \
			fail_tassert(__tres, #e); \
	} while (0)

