#include <lt2/test.h>

b8 any_test_failed = 0;

void after_test(result_t* res) {
	if (res->failed) {
		llogf(NULL, LOG_ALERT, "[{char*}] {u32} failed ({u32} total)", res->name, res->failed, res->count);
		any_test_failed = 1;
	}
	else {
		llogf(NULL, LOG_INFO, "[{char*}] {u32} passed", res->name, res->count);
	}
}

