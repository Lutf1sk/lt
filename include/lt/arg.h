#ifndef LT_ARG_H
#define LT_ARG_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

typedef
struct lt_arg_iterator {
	char** end;
	char** it;
	usz arg_len;
	u8 end_of_opt;
} lt_arg_iterator_t;

lt_arg_iterator_t lt_arg_iterator_create(int argc, char** argv);
b8 lt_arg_next(lt_arg_iterator_t* it);

#define lt_foreach_arg(it_name, argc, argv) for ( lt_arg_iterator_t __it = lt_arg_iterator_create(argc, argv), *it_name = &__it; lt_arg_next(&__it); )

b8 lt_arg_flag(lt_arg_iterator_t* it, char short_key, lstr_t long_key);
b8 lt_arg_str(lt_arg_iterator_t* it, char short_key, lstr_t long_key, char** out);
b8 lt_arg_lstr(lt_arg_iterator_t* it, char short_key, lstr_t long_key, lstr_t* out);
b8 lt_arg_int(lt_arg_iterator_t* it, char short_key, lstr_t long_key, i64* out);
b8 lt_arg_uint(lt_arg_iterator_t* it, char short_key, lstr_t long_key, u64* out);

#endif
