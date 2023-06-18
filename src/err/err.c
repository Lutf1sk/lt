#include <lt/err.h>
#include <lt/debug.h>

static
lstr_t strtab[LT_ERR_MAX] = {
	CLSTR("SUCCESS"),
#define LT_ERR_OP(x) CLSTR(#x),
	LT_FOR_EACH_ERR()
#undef LT_ERR_OP
};

lstr_t lt_err_str(lt_err_t err) {
	LT_ASSERT(err < LT_ERR_MAX);
	return strtab[err];
}

