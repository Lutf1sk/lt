#include <lt/arg.h>
#include <lt/str.h>

typedef
enum lt_arg_stype {
	LT_ARG_BOOL,
	LT_ARG_STR,
	LT_ARG_INT,
	LT_ARG_UINT,
} lt_arg_stype_t;

typedef
struct lt_arg {
	lt_arg_stype_t stype;
	usz count;
	void* out;
} lt_arg_t;

#define ARG(stype, out) (lt_arg_t){ (stype), 0, (out) }

static lt_arg_t args[LT_ARG_MAX];
static lstr_t flags[LT_ARG_MAX];
static usz arg_count = 0;

// ----- Flag registering
static
lt_arg_t* new_flag(lstr_t fl, lt_arg_t arg) {
	LT_ASSERT(arg_count < LT_ARG_MAX);
	args[arg_count] = arg;
	flags[arg_count] = fl;
	return &args[arg_count++];
}

lt_arg_t* lt_arg_bool(lstr_t fl, b8* out) {
	return new_flag(fl, ARG(LT_ARG_BOOL, out));
}

lt_arg_t* lt_arg_str(lstr_t fl, lstr_t* out) {
	return new_flag(fl, ARG(LT_ARG_STR, out));
}

lt_arg_t* lt_arg_int(lstr_t fl, isz* out) {
	return new_flag(fl, ARG(LT_ARG_INT, out));
}

lt_arg_t* lt_arg_uint(lstr_t fl, usz* out) {
	return new_flag(fl, ARG(LT_ARG_UINT, out));
}

// ----- Parsing

void lt_arg_parse(int argc, char** argv) {
	for (int argi = 1; argi < argc; ++argi) {
		if (argv[argi][0] != '-')
			continue;

		char* flag_start = &argv[argi][1];
		lstr_t flag = LSTR(flag_start, strlen(flag_start));

		for (usz i = 0; i < arg_count; ++i) {
			if (!lt_lstr_eq(flag, flags[i]))
				continue;

			lt_arg_t* arg = &args[i];

			if (arg->stype == LT_ARG_BOOL) {
				*(b8*)arg->out = 1;
				++arg->count;
				break;
			}

			if (argi >= argc - 1)
				break;

			++argi;
			lstr_t str_val = LSTR(argv[argi], strlen(argv[argi]));

			++arg->count;

			switch (arg->stype) {
			case LT_ARG_STR: *(lstr_t*)arg->out = str_val; break;
			case LT_ARG_INT: *(isz*)arg->out = lt_lstr_int(str_val); break;
			case LT_ARG_UINT: *(usz*)arg->out = lt_lstr_uint(str_val); break;
			default: break;
			}

			break;
		}
	}
}

// ----- Queries

b8 lt_arg_isset(lt_arg_t* arg) {
	return arg->count != 0;
}


