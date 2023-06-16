#include <lt/io.h>
#include <lt/lt.h>

int lt_assert_failed(lstr_t file, usz line, lstr_t assertion) {
	lt_ferrbf("%S:%uz: Assertion '%S' failed\n", file, line, assertion);
}

int lt_assert_unreachable_failed(lstr_t file, usz line) {
	lt_ferrbf("%S:%uz: Assertion 'Unreachable' failed\n", file, line);
}

