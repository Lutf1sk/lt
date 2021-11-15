#ifndef LT_CONF_H
#define LT_CONF_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

#define LT_FOR_EACH_CONF() \
	LT_CONF_OP(BOOL) \
	LT_CONF_OP(INT) \
	LT_CONF_OP(FLOAT) \
	LT_CONF_OP(STRING) \
	LT_CONF_OP(OBJECT) \
	LT_CONF_OP(ARRAY)

typedef
enum lt_conf_stype {
#define LT_CONF_OP(x) LT_CONF_##x,
	LT_FOR_EACH_CONF()
#undef LT_CONF_OP
} lt_conf_stype_t;

typedef
struct lt_conf {
	lt_conf_stype_t stype;
	lstr_t key;
	usz count;

	union {
		i64 int_val;
		u64 uint_val;
		f64 float_val;
		b8 bool_val;
		char* str_val;

		lt_conf_t* children;
	};
} lt_conf_t;

#define LT_CONF_INIT(stype, key, ...) { (stype), (key), 0, __VA_ARGS__ }
#define LT_CONF(stype, key, ...) ((lt_conf_t)LT_CONF_INIT(stype, key, __VA_ARGS__))

lstr_t conf_type_str(lt_conf_stype_t stype);

b8 lt_conf_parse(lt_conf_t* cf, lstr_t data);

lt_conf_t* lt_conf_find(lt_conf_t* parent, lstr_t key);

i64 lt_conf_int(lt_conf_t* cf, i64 default_);
u64 lt_conf_uint(lt_conf_t* cf, u64 default_);
f64 lt_conf_float(lt_conf_t* cf, f64 default_);
b8 lt_conf_bool(lt_conf_t* cf, b8 default_);
lstr_t lt_conf_str(lt_conf_t* cf, lstr_t default_);

b8 lt_conf_write(lt_conf_t* cf, lt_file_t* file);

void lt_conf_free(lt_conf_t* cf);

#endif
