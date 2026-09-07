#include <lt2/ini.h>
#include <lt2/bits.h>
#include <lt2/str.h>
#define CTYPE_PREFIX lt_
#include <lt2/ctype.h>

#include <assert.h>

#define STRTAB_BLOCKSIZE 8192

static
u32 write_string(ini_t ini[static 1], ls str) {
	usz new_size = ini->strtab_size + str.size;
	if ((new_size ^ ini->strtab_size) & ~(STRTAB_BLOCKSIZE-1)) {
		usz new_capacity = align(new_size, STRTAB_BLOCKSIZE);
		void* new_mem = realloc(ini->strtab, new_capacity);
		assert(new_mem);
		ini->strtab = new_mem;
	}

	u32 write_offset = ini->strtab_size;
	memcpy(ini->strtab + write_offset, str.ptr, str.size);
	ini->strtab_size = new_size;
	return write_offset;
}

// ----- lookups

isz ini_find_section(const ini_t ini[static 1], ls name) {
	for (usz i = 0; i < ini->section_count; ++i) {
		if (lseq(ini_section_name(ini, i), name))
			return i;
	}
	return -1;
}

ls ini_find_value(const ini_t ini[static 1], isz section_i, ls key) {
	if (section_i < 0)
		return lls(NULL, 0);

	ini_section_t* section = &ini->sections[section_i];
	for (ini_line_t* it = section->lines, *end = it + section->line_count; it < end; ++it) {
		if (it->type == INI_LINE_VALUE && lseq(ini_line_key(ini, it), key))
			return ini_line_value(ini, it);
	}
	return lls(NULL, 0);
}

isz ini_find_value_line(const ini_t ini[static 1], isz section_i, ls key) {
	if (section_i < 0)
		return -1;

	ini_section_t* section = &ini->sections[section_i];
	for (ini_line_t* it = section->lines, *end = it + section->line_count; it < end; ++it) {
		if (it->type == INI_LINE_VALUE && lseq(ini_line_key(ini, it), key))
			return it - section->lines;
	}
	return -1;
}

// ----- addition

#define SECTION_BLOCKSIZE 256

usz ini_add_section(ini_t ini[static 1], ls name) {
	if ((ini->section_count & (SECTION_BLOCKSIZE-1)) == 0) {
		usz new_capacity = (usz)ini->section_count + SECTION_BLOCKSIZE;
		void* new_mem = realloc(ini->sections, new_capacity * sizeof(ini_section_t));
		assert(new_mem);
		ini->sections = new_mem;
	}

	ini->sections[ini->section_count] = (ini_section_t) {
		.name_offset = write_string(ini, name),
		.name_length = name.size,
	};
	return ini->section_count++;
}

#define ENTRY_BLOCKSIZE 128

isz ini_add_line(ini_t ini[static 1], isz section_i, ini_line_t line[static 1]) {
	if (section_i < 0)
		return -1;

	ini_section_t* section = &ini->sections[section_i];
	if ((section->line_count & (ENTRY_BLOCKSIZE-1)) == 0) {
		usz new_capacity = section->line_count + ENTRY_BLOCKSIZE;
		void* new_mem = realloc(section->lines, new_capacity * sizeof(ini_line_t));
		assert(new_mem);
		section->lines = new_mem;
	}

	section->lines[section->line_count] = *line;
	return section->line_count++;
}

isz ini_add_value(ini_t ini[static 1], isz section_i, ls key, ls value) {
	return ini_add_line(ini, section_i, &(ini_line_t) {
		.type = INI_LINE_VALUE,
		.key_offset = write_string(ini, key),
		.key_length = key.size,
		.offset = write_string(ini, value),
		.length = value.size,
	});
}

isz ini_set_value(ini_t ini[static 1], isz section_i, ls key, ls value) {
	isz existing_line = ini_find_value_line(ini, section_i, key);
	if (existing_line < 0)
		return ini_add_value(ini, section_i, key, value);

	ini_line_t* line = &ini->sections[section_i].lines[existing_line];
	line->offset = write_string(ini, value);
	line->length = value.size;
	return existing_line;
}

// ----- removal

void ini_remove_line(ini_t ini[static 1], isz section_i, isz line_i) {
	if (section_i < 0 || line_i < 0)
		return;
	ini_section_t* section = &ini->sections[section_i];
	memmove(section->lines + line_i, section->lines + line_i + 1, (section->line_count - line_i - 1) * sizeof(ini_line_t));
	--section->line_count;
}

// ----- parsing

INLINE
u8* skip_space(u8* it, u8* end) {
	while (it < end && lt_isspace(*it))
		++it;
	return it;
}

INLINE
u8* skip_nonlf_space(u8* it, u8* end) {
	while (it < end && (*it == ' ' || *it == '\t' || *it == '\v'))
		++it;
	return it;
}

INLINE
u8* skip_line(u8* it, u8* end) {
	while (it < end) {
		if (*it == '\n')
			return ++it;
		++it;
	}
	return it;
}

ini_t ini_parse(ls str, err* err) {
	ini_t ini = { 0 };
	ini.strtab = malloc(STRTAB_BLOCKSIZE);
	assert(ini.strtab);
	u32 section_i = ini_add_section(&ini, lls(NULL, 0));

	u8* it = str.ptr, *end = it + str.size;
	while (it < end) {
		it = skip_nonlf_space(it, end);

		if (it >= end || *it == '\n') {
			++it;
			ini_add_line(&ini, section_i, &(ini_line_t) {
				.type = INI_LINE_EMPTY,
			});
			continue;
		}

		if (*it == ';') {
			u8* start = ++it;
			it = skip_line(it, end);
			ls text = lstrim_right(lsrange(start, it));

			ini_add_line(&ini, section_i, &(ini_line_t) {
				.type   = INI_LINE_COMMENT,
				.offset = write_string(&ini, text),
				.length = text.size,
			});
			continue;
		}

		if (*it == '[') {
			u8* start = ++it;
			for (;;) {
				if (it >= end || *it == '\n') {
					throw(err, ERR_BAD_SYNTAX, "expected '[' before end of line");
					goto err;
				}
				if (*it == ']')
					break;
				++it;
			}
			ls name = lstrim(lsrange(start, it));

			++it;
			if (it < end && *it == '\n')
				++it;

			section_i = ini_add_section(&ini, name);
			continue;
		}

		u8* start = it;
		for (;;) {
			if (it >= end || *it == '\n') {
				throw(err, ERR_BAD_SYNTAX, "expected '=' before end of file");
				goto err;
			}
			if (*it == '=')
				break;
			++it;
		}
		ls key = lstrim(lsrange(start, it));

		start = ++it;
		it = skip_line(it, end);
		ls value = lstrim_right(lsrange(start, it));

		ini_add_line(&ini, section_i, &(ini_line_t) {
			.type       = INI_LINE_VALUE,
			.offset     = write_string(&ini, value),
			.length     = value.size,
			.key_offset = write_string(&ini, key),
			.key_length = key.size,
		});
	}

	return ini;

err:
	ini_free(&ini);
	return ini;
}

ini_t ini_load(ls path, err* err) {
	ls file_data = fmapall(path, R, err);
	ini_t ini = ini_parse(file_data, err);
	if (file_data.ptr)
		free(file_data.ptr);
	return ini;
}

void ini_free(ini_t ini[static 1]) {
	for (ini_section_t* it = ini->sections, *end = it + ini->section_count; it < end; ++it) {
		if (it->lines)
			free(it->lines);
	}
	if (ini->sections)
		free(ini->sections);
	if (ini->strtab)
		free(ini->strtab);
}


void ini_write(const ini_t ini[static 1], file_handle file) {
	for (usz section_i = 0; section_i < ini->section_count; ++section_i) {
		ini_section_t* section = &ini->sections[section_i];
		ls name = ini_section_name(ini, section_i);
		if (!name.size)
			continue;

		lfprintf(file, "[%S]\n", name);

		for (ini_line_t* line_it = section->lines, *line_end = line_it + section->line_count; line_it < line_end; ++line_it) {
			switch (line_it->type) {
			case INI_LINE_EMPTY:   lfprintf(file, "\n"); break;
			case INI_LINE_COMMENT: lfprintf(file, ";%S\n",   ini_line_value(ini, line_it)); break;
			case INI_LINE_VALUE:   lfprintf(file, "%S=%S\n", ini_line_key(ini, line_it), ini_line_value(ini, line_it)); break;
			}
		}
	}
}

