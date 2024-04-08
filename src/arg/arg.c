#include <lt/arg.h>
#include <lt/str.h>
#include <lt/debug.h>

lt_arg_iterator_t lt_arg_iterator_create(int argc, char* argv[static argc]) {
	lt_arg_iterator_t it;
	it.end = argv + argc;
	it.it = argv;
	it.arg_len = 0;
	it.end_of_opt = 0;
	return it;
}

b8 lt_arg_next(lt_arg_iterator_t it[static 1]) {
	if (++it->it >= it->end)
		return 0;
	it->arg_len = strlen(*it->it);
	if (!it->end_of_opt && it->arg_len == 2 && memcmp(*it->it, "--", 2) == 0) {
		it->end_of_opt = 1;
		return lt_arg_next(it);
	}
	return 1;
}

static
char* find_long_val(lt_arg_iterator_t it[static 1], lstr_t key) {
	if (!key.len)
		return NULL;

	LT_ASSERT(key.str[0] != '-');
	if (it->arg_len < 3 || memcmp(*it->it, "--", 2) != 0)
		return NULL;

	lstr_t arg = LSTR(*it->it + 2, it->arg_len - 2);
	if (lt_lseq(arg, key)) {
		if (!lt_arg_next(it))
			lt_ferrf("missing argument to '--%S'\n", key); // !!
		return *it->it;
	}

	if (lt_lsprefix(arg, key) && arg.str[key.len] == '=')
		return arg.str + key.len + 1;
	return NULL;
}

static
char* find_short_val(lt_arg_iterator_t it[static 1], char key) {
	if (!key)
		return NULL;

	LT_ASSERT(key != '-');
	if (it->arg_len < 2 || **it->it != '-')
		return NULL;

	lstr_t arg = LSTR(*it->it + 1, it->arg_len - 1);
	if (arg.len == 1 && arg.str[0] == key) {
		if (!lt_arg_next(it))
			lt_ferrf("missing argument to '-%c'\n", key); // !!
		return *it->it;
	}

	if (lt_lsprefix(arg, LSTR(&key, 1)) && arg.len > 1)
		return arg.str + 1;
	return NULL;
}

b8 lt_arg_flag(lt_arg_iterator_t it[static 1], char short_key, lstr_t long_key) {
	lstr_t arg = LSTR(*it->it, it->arg_len);

	char short_flag[2] = { '-', short_key };
	if (short_key && arg.len == 2 && memcmp(arg.str, short_flag, 2) == 0)
		return 1;

	if (arg.len < 3 || memcmp(arg.str, "--", 2) != 0)
		return 0;
	return lt_lseq(LSTR(arg.str + 2, arg.len - 2), long_key);
}

b8 lt_arg_str(lt_arg_iterator_t it[static 1], char short_key, lstr_t long_key, char* out[static 1]) {
	if (it->end_of_opt)
		return 0;

	char* arg = find_short_val(it, short_key);
	if (!arg)
		arg = find_long_val(it, long_key);
	if (arg)
		*out = arg;
	return !!arg;
}

b8 lt_arg_lstr(lt_arg_iterator_t it[static 1], char short_key, lstr_t long_key, lstr_t out[static 1]) {
	if (it->end_of_opt)
		return 0;

	char* arg = find_short_val(it, short_key);
	if (!arg)
		arg = find_long_val(it, long_key);
	if (arg)
		*out = LSTR(arg, strlen(arg));
	return !!arg;
}

b8 lt_arg_int(lt_arg_iterator_t it[static 1], char short_key, lstr_t long_key, i64 out[static 1]) {
	// !!
	return 0;
}

b8 lt_arg_uint(lt_arg_iterator_t it[static 1], char short_key, lstr_t long_key, u64 out[static 1]) {
	// !!
	return 0;
}

