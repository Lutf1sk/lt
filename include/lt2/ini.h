#pragma once

#include <lt2/common.h>

#define INI_LINE_EMPTY   0
#define INI_LINE_VALUE   1
#define INI_LINE_COMMENT 2

typedef struct ini_line {
	u8 type;
	u8 key_length;
	u16 length;
	u32 offset;
	u32 key_offset;
} ini_line_t;

typedef struct ini_section {
	u32 name_offset;
	u16 name_length;
	u16 line_count;
	ini_line_t* lines;
} ini_section_t;

typedef struct ini {
	ini_section_t* sections;
	usz section_count;

	char* strtab;
	usz strtab_size;
} ini_t;

INLINE
ls ini_section_name(const ini_t ini[static 1], isz section_i) {
	if (section_i < 0)
		return lls(NULL, 0);
	return lls(ini->strtab + ini->sections[section_i].name_offset, ini->sections[section_i].name_length);
}

INLINE
ls ini_line_key(const ini_t ini[static 1], ini_line_t line[static 1]) {
	return lls(ini->strtab + line->key_offset, line->key_length);
}

INLINE
ls ini_line_value(const ini_t ini[static 1], ini_line_t line[static 1]) {
	return lls(ini->strtab + line->offset, line->length);
}

isz ini_find_section(const ini_t ini[static 1], ls name);
ls ini_find_value(const ini_t ini[static 1], isz section_i, ls key);
isz ini_find_value_line(const ini_t ini[static 1], isz section_i, ls key);

isz ini_add_section(ini_t ini[static 1], ls name, err* err);
isz ini_add_line(ini_t ini[static 1], isz section_i, ini_line_t line[static 1], err* err);
isz ini_add_value(ini_t ini[static 1], isz section_i, ls key, ls value, err* err);
isz ini_set_value(ini_t ini[static 1], isz section_i, ls key, ls value, err* err);

void ini_remove_line(ini_t ini[static 1], isz section_i, isz line_i);

INLINE
void ini_remove_value(ini_t ini[static 1], isz section_i, ls key) {
	ini_remove_line(ini, section_i, ini_find_value_line(ini, section_i, key));
}

ini_t ini_parse(ls str, err* err);
ini_t ini_load(ls path, err* err);
void ini_free(ini_t ini[static 1]);

void ini_write(const ini_t ini[static 1], file_handle file);

