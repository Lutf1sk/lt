#include <lt2/bits.h>
#include <lt2/str.h>
#include <lt2/log.h>

#include <time.h>

ls log_mapping;
log_header* log_file_header;

u8* log_strtab;
usz log_strtab_mask;

log_entry* log_enttab;
usz log_enttab_mask;

thread_local usz log_field_count = 0;
thread_local u64 log_fields[MAX_LOG_FIELDS];

b8 log_create(ls path, u32 max_entries, u32 strtab_size, err* err) {
	if (!is_nzpow2(max_entries) || !is_nzpow2(strtab_size)) {
		throw(err, ERR_BAD_ARGUMENT, "invalid table size(s)");
		return 0;
	}

	file_handle f = fcreate(path, RW, err);
	if (f < 0)
		return 0;
	if (!fsetsize(f, sizeof(log_header) + max_entries * sizeof(log_entry) + strtab_size, err)) {
		lfclose(f, err_ignore);
		return 0;
	}

	b8 success = lfwrite(f, &(log_header) {
		.version = 0,
		.header_size = sizeof(log_header),
		.strtab_size = strtab_size,
		.max_entries = max_entries,
	}, sizeof(log_header), err);

	lfclose(f, err);
	return success;
}

b8 log_open(ls path, u8 mode, err* err) {
	ls mapping;
	if (mode & W) {
		mapping = fmapall(path, mode, err_ignore);
		if (!mapping.size) {
			if (!log_create(path, MB(1), MB(32), err))
				return 0;
			mapping = fmapall(path, mode, err);
		}
	}
	else {
		mapping = fmapall(path, mode, err);
	}
	if (!mapping.size)
		return 0;

	if (mapping.size < sizeof(log_header)) {
		throw(err, ERR_CORRUPT, "log file does not contain a valid header");
		goto err0;
	}

	log_header* header_tmp = (log_header*)mapping.ptr;
	if (header_tmp->version != 0 || header_tmp->header_size < sizeof(log_header)) {
		throw(err, ERR_UNSUPPORTED, "log file version is unknown or invalid");
		goto err0;
	}

	usz total_size = (usz)header_tmp->header_size + (usz)header_tmp->strtab_size + (usz)header_tmp->max_entries * sizeof(log_entry);
	if (total_size > mapping.size || !is_nzpow2(header_tmp->strtab_size) || !is_nzpow2(header_tmp->max_entries)) {
		throw(err, ERR_CORRUPT, "invalid table size(s)");
		goto err0;
	}

	log_mapping     = mapping;
	log_file_header = header_tmp;
	log_strtab_mask = header_tmp->strtab_size - 1;
	log_enttab_mask = header_tmp->max_entries - 1;
	log_strtab      = (u8*)(log_mapping.ptr + header_tmp->header_size);
	log_enttab      = (log_entry*)(log_strtab + header_tmp->strtab_size);
	log_field_count = 0;
	return 1;

err0: funmap(mapping, err);
	return 0;
}

void log_close(err* err) {
	funmap(log_mapping, err);
}

static ls severity_names[] = {
	[LOG_EMERG]  = ls("EMERG!\x1b[0;1m"),
	[LOG_ALERT]  = ls("ALERT!\x1b[0;1m"),
	[LOG_CRIT]   = ls(" CRIT!\x1b[0;1m"),
	[LOG_ERR]    = ls("   ERR\x1b[0m"),
	[LOG_WARN]   = ls("  WARN\x1b[0m"),
	[LOG_NOTICE] = ls("NOTICE\x1b[0m"),
	[LOG_INFO]   = ls("  INFO\x1b[0m"),
	[LOG_DEBUG]  = ls(" DEBUG\x1b[0m"),
};

static ls severity_prefixes[] = {
	[LOG_EMERG]  = ls("\x1b[41;97;1m"),
	[LOG_ALERT]  = ls("\x1b[101;30;1m"),
	[LOG_CRIT]   = ls("\x1b[41;30;1m"),
	[LOG_ERR]    = ls("\x1b[31;1m"),
	[LOG_WARN]   = ls("\x1b[93;1m"),
	[LOG_NOTICE] = ls("\x1b[94;1m"),
	[LOG_INFO]   = ls("\x1b[97;1m"),
	[LOG_DEBUG]  = ls("\x1b[37;1m"),
};

void print_log_entry(i32 index) {
	log_entry* ent = &log_enttab[index];
	u8* ent_str_start = log_strtab + ent->offset;
	u8 sev = ent->info & 7;
	usz fields_size = ent->fields * sizeof(*log_fields);
	lprintf("{ls}{dt64} {ls} ", severity_prefixes[sev], ent->time, severity_names[sev]);
	if (ent->fields) {
		for (usz i = 0; i < ent->fields; ++i)
			lprintf("\x1b[4;3{usz}m{u64}\x1b[0m ", (i & 3) + 3,((u64*)ent_str_start)[i]);
	}
	lprintf("{ls}\x1b[0m\n", lls(ent_str_start + fields_size, ent->size - fields_size - 1));
}

static
i32 index_by_offset(u32 offs) {
	if (!log_file_header->entry_count)
		return -1;

	u32 r = 0;
	u32 l = log_file_header->entry_count - 1;
	u8 above_top = offs >= log_file_header->strtab_top;
	do {
		u32 m = (r + l) >> 2;
		log_entry* ent = &log_enttab[(log_file_header->enttab_first + m) & log_enttab_mask];
		if (above_top == (ent->offset >= log_file_header->strtab_top)) {
			if (offs < ent->offset) {
				r = m;
				continue;
			}
			if (offs < ent->offset + ent->size)
				return m;
			l = m;
		}
		else if (above_top) {
			l = m;
		}
		else {
			r = m;
		}
	} while (r != l);
	return -1;
}

i32 vlogf(u8 info, const char* fmt, va_list args) {
	if (!log_file_header)
		return -1;

	va_list args2;
	va_copy(args2, args);

	usz fields_size = log_field_count * sizeof(*log_fields);
	usz msglen = fields_size + vllenf(fmt, args) + 1;
	if (msglen > log_file_header->strtab_size) {
		throw(err_warn, ERR_LIMIT_EXCEEDED, "log entry exceeds maximum size");
		va_end(args2);
		return -1;
	}

	u32 advance = 0;
	if (msglen + log_file_header->strtab_top > log_file_header->strtab_size) {
		for (; advance < log_file_header->entry_count; ++advance) {
			usz offs = log_enttab[(log_file_header->enttab_first + advance) & log_enttab_mask].offset;
			if (offs < log_file_header->strtab_top)
				break;
		}
		log_file_header->strtab_top = 0;
	}

	for (; advance < log_file_header->entry_count; ++advance) {
		usz index = (log_file_header->enttab_first + advance) & log_enttab_mask;
		usz offs = log_enttab[index].offset;
		usz size = log_enttab[index].size;
		if (log_file_header->strtab_top + msglen <= offs || log_file_header->strtab_top >= offs + size)
			break;
	}

	memcpy(log_strtab + log_file_header->strtab_top, log_fields, fields_size);
	u8* str_start = log_strtab + log_file_header->strtab_top + fields_size;
	ls msg = vlsprintf(lls(str_start, msglen), fmt, args2);
	va_end(args2);
	str_start[msg.size] = 0;

	u32 ent_index = (log_file_header->enttab_first + log_file_header->entry_count) & log_enttab_mask;
	if (!advance && log_file_header->entry_count >= log_file_header->max_entries)
		++advance;
	log_file_header->entry_count += 1 - advance;
	log_file_header->enttab_first = (log_file_header->enttab_first + advance) & log_enttab_mask;

	log_enttab[ent_index] = (log_entry) {
		.info   = info,
		.fields = log_field_count,
		.size   = msglen,
		.offset = log_file_header->strtab_top,
		.time   = time(NULL)
	};
	log_file_header->strtab_top += msglen;
	return ent_index;
}

i32 llogf(u8 info, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	i32 res = vlogf(info, fmt, args);
	va_end(args);
	return res;
}

