#pragma once

#include <lt2/common.h>

typedef struct ini_section {
	usz first_occurence;
	ls  name;
} ini_section_t;

typedef struct ini_key {
	usz section;
	ls  key;
	ls  value;
} ini_key_t;

typedef struct ini {
	usz section_count;
	ini_section_t* sections;
	usz key_count;
	ini_key_t* keys;

	vmap_t mappings[2];
} ini_t;

ini_t parse_ini(ls str, err* err);

// returns negative ERR_NOT_FOUND if the section does not exist
isz ini_find_section(const ini_t ini[static 1], ls name);

// returns null if the key does not exist, or if the section is invalid
ini_key_t* ini_find_key(const ini_t ini[static 1], isz section, ls name);

// the following functions return the value of the requested key.
// if the key does not exist, or if the section is invalid, it returns the default value instead.
// if the key exists, but could not be converted to the correct format, the default value is returned, and 'warn' is called.
ls  ini_find_str(const ini_t ini[static 1], isz section, ls name, ls  default_);
u8  ini_find_u8 (const ini_t ini[static 1], isz section, ls name, u8  default_);
u16 ini_find_u16(const ini_t ini[static 1], isz section, ls name, u16 default_);
u32 ini_find_u32(const ini_t ini[static 1], isz section, ls name, u32 default_);
u64 ini_find_u64(const ini_t ini[static 1], isz section, ls name, u64 default_);
i8  ini_find_i8 (const ini_t ini[static 1], isz section, ls name, i8  default_);
i16 ini_find_i16(const ini_t ini[static 1], isz section, ls name, i16 default_);
i32 ini_find_i32(const ini_t ini[static 1], isz section, ls name, i32 default_);
i64 ini_find_i64(const ini_t ini[static 1], isz section, ls name, i64 default_);
f32 ini_find_f32(const ini_t ini[static 1], isz section, ls name, f32 default_);
f64 ini_find_f64(const ini_t ini[static 1], isz section, ls name, f64 default_);

// the following functions return the value of the requested key.
// if the key does not exist, or the value could not be converted, an error is omitted, and a zeroed value is returned.
ls  ini_require_str(const ini_t ini[static 1], isz section, ls name, err* err);
u8  ini_require_u8 (const ini_t ini[static 1], isz section, ls name, err* err);
u16 ini_require_u16(const ini_t ini[static 1], isz section, ls name, err* err);
u32 ini_require_u32(const ini_t ini[static 1], isz section, ls name, err* err);
u64 ini_require_u64(const ini_t ini[static 1], isz section, ls name, err* err);
i8  ini_require_i8 (const ini_t ini[static 1], isz section, ls name, err* err);
i16 ini_require_i16(const ini_t ini[static 1], isz section, ls name, err* err);
i32 ini_require_i32(const ini_t ini[static 1], isz section, ls name, err* err);
i64 ini_require_i64(const ini_t ini[static 1], isz section, ls name, err* err);
f32 ini_require_f32(const ini_t ini[static 1], isz section, ls name, err* err);
f64 ini_require_f64(const ini_t ini[static 1], isz section, ls name, err* err);

