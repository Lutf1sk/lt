#include <lt/io.h>
#include <lt/lt.h>

int lt_assert_failed(lstr_t file, int line, lstr_t assertion) {
	lt_ferrbf("%S:%id: Assertion '%S' failed\n", file, line, assertion);
}

int lt_assert_unreachable_failed(lstr_t file, int line) {
	lt_ferrbf("%S:%id: Assertion 'Unreachable' failed\n", file, line);
}

