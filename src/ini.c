#include <ctype.h>

#include <lt2/str.h>
#include <lt2/ini.h>

ini parse_ini(ls str, err* err) {
	ini ini = {
		.mappings = {
			{ .size = KB(4), .guard_size = 1, .permit = RW, .out = (void**)&ini.sections },
			{ .size = KB(4), .guard_size = 1, .permit = RW, .out = (void**)&ini.keys },
		}
	};

	vmap(ini.mappings, COUNT_OF(ini.mappings), 0, err);

	usz section = 0;

	u8* start = str.ptr;
	u8* it    = start;
	u8* end   = it + str.size;
	while (it < end) {
		u8 c = *it;

		while (isspace(c)) {
			if (++it >= end)
				return ini;
			c = *it;
		}

		if (c == '#' || c == ';') {
			while (it < end && *it != '\n')
				++it;
			++it;
			continue;
		}

		if (c == '[') {
			u8* line = ++it;
			for (;;) {
				if (it >= end) {
					throw(err, ERR_BAD_SYNTAX, "expected ']' before end of file");
					return ini;
				}
				if (*it == ']')
					break;
				if (*it++ == '\n') {
					throw(err, ERR_BAD_SYNTAX, "expected ']' before line break");
					return ini;
				}
			}

			ls name = lsrange(line, it++);
			if (ini_find_section(&ini, name) >= 0) {
				throw(err, ERR_BAD_SYNTAX, "duplicate ini section name");
				return ini;
			}

			section = ++ini.section_count;
			ini.sections[section] = (ini_section) {
				.name = name,
				.first_occurence = ini.key_count,
			};
			continue;
		}

		u8* line = it;
		while (*it != '=') {
			if (*it == '\n') {
				throw(err, ERR_BAD_SYNTAX, "expected '=' before line break");
				return ini;
			}
			if (++it >= end) {
				throw(err, ERR_BAD_SYNTAX, "expected '=' before end of file");
				return ini;
			}
		}

		ls key = lstrim_right(lsrange(line, it++));
		u32 key_idx = ini.key_count++;
		ini.keys[key_idx].section = section;
		ini.keys[key_idx].key = key;

		while (it < end && *it != '\n' && isspace(*it))
			++it;
		line = it;
		while (it < end && *it != '\n')
			++it;
		ini.keys[key_idx].value = lstrim_right(lsrange(line, it));
	}
	return ini;
}

isz ini_find_section(const ini ini[static 1], ls name) {
	ini_section* sections = ini->sections;
	isz section_count = ini->section_count;
	for (isz i = 1; i <= section_count; ++i) {
		if (lseq(sections[i].name, name))
			return i;
	}
	return -1;
}

ini_key* ini_find_key(const ini ini[static 1], isz section, ls name) {
	if (section < 0)
		return NULL;

	ini_key* it  = ini->keys + ini->sections[section].first_occurence;
	ini_key* end = it + ini->key_count;
	for (; it < end && it->section == section; ++it) {
		if (lseq(it->key, name))
			return it;
	}
	return NULL;
}

ls ini_find_str(const ini ini[static 1], isz section, ls name, ls default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return key->value;
}

// !! all of the following functions should check for overflow

u8 ini_find_u8(const ini ini[static 1], isz section, ls name, u8 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstou(key->value, err_ignore);
}

u16 ini_find_u16(const ini ini[static 1], isz section, ls name, u16 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstou(key->value, err_ignore);
}

u32 ini_find_u32(const ini ini[static 1], isz section, ls name, u32 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstou(key->value, err_ignore);
}

u64 ini_find_u64(const ini ini[static 1], isz section, ls name, u64 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstou(key->value, err_ignore);
}

i8 ini_find_i8(const ini ini[static 1], isz section, ls name, i8 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstoi(key->value, err_ignore);
}

i16 ini_find_i16(const ini ini[static 1], isz section, ls name, i16 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstoi(key->value, err_ignore);
}

i32 ini_find_i32(const ini ini[static 1], isz section, ls name, i32 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstoi(key->value, err_ignore);
}

i64 ini_find_i64(const ini ini[static 1], isz section, ls name, i64 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstoi(key->value, err_ignore);
}

f32 ini_find_f32(const ini ini[static 1], isz section, ls name, f32 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstof(key->value, err_ignore);
}

f64 ini_find_f64(const ini ini[static 1], isz section, ls name, f64 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lstof(key->value, err_ignore);
}

b8 ini_find_b8(const ini ini[static 1], isz section, ls name, b8 default_) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL)
		return default_;
	return lseq(key->value, ls("1")) || !lseq(key->value, ls("true"));
}

ls ini_require_str(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return ls("");
	}
	return key->value;
}

u8 ini_require_u8(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

u16 ini_require_u16(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

u32 ini_require_u32(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

u64 ini_require_u64(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

i8 ini_require_i8(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

i16 ini_require_i16(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

i32 ini_require_i32(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

i64 ini_require_i64(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

f32 ini_require_f32(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

f64 ini_require_f64(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lstou(key->value, err);
}

b8 ini_require_b8(const ini ini[static 1], isz section, ls name, err* err) {
	ini_key* key = ini_find_key(ini, section, name);
	if (key == NULL) {
		throw(err, ERR_NOT_FOUND, "ini key not found"); // !!
		return 0;
	}
	return lseq(key->value, ls("1")) || !lseq(key->value, ls("true"));
}

