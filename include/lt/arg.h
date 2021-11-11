#ifndef LT_ARG_H
#define LT_ARG_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

#define LT_ARG_MAX 256

lt_arg_t* lt_arg_bool(lstr_t fl, b8* out);
lt_arg_t* lt_arg_str(lstr_t fl, lstr_t* out);
lt_arg_t* lt_arg_int(lstr_t fl, isz* out);
lt_arg_t* lt_arg_uint(lstr_t fl, usz* out);

void lt_arg_parse(int argc, char** argv);

b8 lt_arg_isset(lt_arg_t* arg);

#endif
