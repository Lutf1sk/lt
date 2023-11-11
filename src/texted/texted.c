#include <lt/texted.h>
#include <lt/darr.h>
#include <lt/math.h>
#include <lt/ctype.h>
#include <lt/text.h>
#include <lt/str.h>
#include <lt/mem.h>

static
void merge_line(lt_texted_t* ed, usz line) {
	usz dst_idx = line;
	usz src_idx = line + 1;
	lt_darr_insert(ed->lines[dst_idx], lt_darr_count(ed->lines[dst_idx]), ed->lines[src_idx], lt_darr_count(ed->lines[src_idx]));
	lt_darr_destroy(ed->lines[src_idx]);
	lt_darr_erase(ed->lines, src_idx, 1);
}

static
void newline(lt_texted_t* ed, usz posy) {
	lt_darr(char) new_line = lt_darr_create(char, 32, lt_darr_head(ed->lines)->alloc);
	lt_darr_insert(ed->lines, posy, &new_line, 1);
}

static
void split(lt_texted_t* ed, usz posy, usz posx) {
	usz src_idx = posy;
	usz dst_idx = posy + 1;

	newline(ed, dst_idx);

	char* move_start_src = &ed->lines[src_idx][posx];
	usz move_count = lt_darr_count(ed->lines[src_idx]) - posx;
	lt_darr_insert(ed->lines[dst_idx], 0, move_start_src, move_count);
	lt_darr_erase(ed->lines[src_idx], posx, move_count);
}

static
void sync_tx(lt_texted_t* ed) {
	if (ed->find_visual_x)
		ed->target_x = ed->find_visual_x(ed->usr, lt_texted_line_str(ed, ed->cursor_y), ed->cursor_x);
	else
		ed->target_x = ed->cursor_x;
}

static
void goto_tx(lt_texted_t* ed) {
	if (ed->find_cursor_x)
		ed->cursor_x = ed->find_cursor_x(ed->usr, lt_texted_line_str(ed, ed->cursor_y), ed->target_x);
	else
		ed->cursor_x = lt_min_usz(ed->target_x, lt_darr_count(ed->lines[ed->cursor_y]));
}

static
void sync_s(lt_texted_t* ed) {
	ed->select_y = ed->cursor_y;
	ed->select_x = ed->cursor_x;
}

static
void goto_selection_start(lt_texted_t* ed) {
	usz x1, y1;
	lt_texted_get_selection(ed, &x1, &y1, NULL, NULL);
	ed->select_y = ed->cursor_y = y1;
	ed->select_x = ed->cursor_x = x1;
}

static
void goto_selection_end(lt_texted_t* ed) {
	usz x2, y2;
	lt_texted_get_selection(ed, NULL, NULL, &x2, &y2);
	ed->select_y = ed->cursor_y = y2;
	ed->select_x = ed->cursor_x = x2;
}

lt_err_t lt_texted_create(lt_texted_t* ed, lt_alloc_t* alloc) {
	ed->select_y = ed->cursor_y = 0;
	ed->select_x = ed->cursor_x = ed->target_x = 0;
	ed->usr = NULL;
	ed->find_visual_x = NULL;
	ed->find_cursor_x = NULL;
	ed->lines = lt_darr_create(lt_darr(char), 128, alloc);
	lt_darr_push(ed->lines, lt_darr_create(char, 32, alloc));
	return LT_SUCCESS;
}

void lt_texted_destroy(lt_texted_t* ed) {
	for (usz i = 0; i < lt_darr_count(ed->lines); ++i)
		lt_darr_destroy(ed->lines[i]);
	lt_darr_destroy(ed->lines);
}

void lt_texted_clear(lt_texted_t* ed) {
	lt_darr(char) first_line = ed->lines[0];
	lt_darr_clear(first_line);

	for (usz i = 1; i < lt_darr_count(ed->lines); ++i)
		lt_darr_destroy(ed->lines[i]);

	lt_darr_clear(ed->lines);
	lt_darr_push(ed->lines, first_line);

	ed->cursor_y = 0;
	ed->cursor_x = 0;
	sync_s(ed);
	sync_tx(ed);
}

isz lt_texted_write_contents(lt_texted_t* ed, lt_io_callback_t callb, void* usr) {
	isz bytes = callb(usr, ed->lines[0], lt_darr_count(ed->lines[0]));
	if (bytes < 0)
		return bytes;

	for (usz i = 1; i < lt_darr_count(ed->lines); ++i) {
		isz res = callb(usr, "\n", 1);
		if (res < 0)
			return res;
		bytes += res;

		res = callb(usr, ed->lines[i], lt_darr_count(ed->lines[i]));
		if (res < 0)
			return res;
		bytes += res;

	}

	return bytes;
}

isz lt_texted_write_range(lt_texted_t* ed, usz x1, usz y1, usz x2, usz y2, lt_io_callback_t callb, void* usr) {
	LT_ASSERT(y1 >= 0 && y1 < lt_darr_count(ed->lines));
	LT_ASSERT(y2 >= 0 && y2 < lt_darr_count(ed->lines));
	LT_ASSERT(x1 >= 0 && x1 <= lt_darr_count(ed->lines[y1]));
	LT_ASSERT(x2 >= 0 && x2 <= lt_darr_count(ed->lines[y2]));

	if (y2 == y1)
		return callb(usr, ed->lines[y1] + x1, x2 - x1);

	isz bytes = callb(usr, ed->lines[y1] + x1, lt_darr_count(ed->lines[y1]) - x1);
	if (bytes < 0)
		return bytes;

	isz res = callb(usr, "\n", 1);
	if (res < 0)
		return res;
	bytes += res;

	usz first_covered_y = y1 + 1;
	for (usz y = first_covered_y; y < y2; ++y) {
		if ((res = callb(usr, ed->lines[y], lt_darr_count(ed->lines[y]))) < 0)
			return res;
		bytes += res;

		if ((res = callb(usr, "\n", 1)) < 0)
			return res;
		bytes += res;
	}

	if ((res = callb(usr, ed->lines[y2], x2)) < 0)
		return res;
	bytes += res;

	return bytes;
}

void lt_texted_erase_range(lt_texted_t* ed, usz x1, usz y1, usz x2, usz y2) {
	if (y2 == y1) {
		lt_darr_erase(ed->lines[y1], x1, x2 - x1);
		return;
	}

	usz first_covered_y = y1 + 1;
	for (usz y = first_covered_y; y < y2; ++y)
		lt_darr_destroy(ed->lines[y]);

	lt_darr_erase(ed->lines[y1], x1, lt_darr_count(ed->lines[y1]) - x1);
	lt_darr_erase(ed->lines[y2], 0, x2);

	lt_darr_erase(ed->lines, first_covered_y, y2 - first_covered_y);

	merge_line(ed, y1);
}

b8 lt_texted_selection_present(lt_texted_t* ed) {
	return !(ed->select_y == ed->cursor_y && ed->select_x == ed->cursor_x);
}

b8 lt_texted_get_selection(lt_texted_t* ed, usz* out_x1, usz* out_y1, usz* out_x2, usz* out_y2) {
	usz y1 = ed->cursor_y;
	usz x1 = ed->cursor_x;
	usz y2 = ed->select_y;
	usz x2 = ed->select_x;

	if (y1 > y2 || (y1 == y2 && x1 > x2)) {
		y2 = ed->cursor_y;
		x2 = ed->cursor_x;
		y1 = ed->select_y;
		x1 = ed->select_x;
	}

	if (out_x1)
		*out_x1 = x1;
	if (out_y1)
		*out_y1 = y1;
	if (out_x2)
		*out_x2 = x2;
	if (out_y2)
		*out_y2 = y2;

	return !(y1 == y2 && x1 == x2);
}

isz lt_texted_write_selection(lt_texted_t* ed, lt_io_callback_t callb, void* usr) {
	usz x1, y1, x2, y2;
	if (!lt_texted_get_selection(ed, &x1, &y1, &x2, &y2))
		return 0;
	return lt_texted_write_range(ed, x1, y1, x2, y2, callb, usr);
}

usz lt_texted_selection_len(lt_texted_t* ed) {
	usz start_y, start_x, end_y, end_x;
	lt_texted_get_selection(ed, &start_y, &start_x, &end_y, &end_x);

	usz len = 0;
	for (usz i = start_y, j = start_x; i <= end_y; ++i) {
		if (i == end_y)
			len += end_x - j;
		else
			len += lt_texted_line_len(ed, i) - j + 1;
		j = 0;
	}

	return len;
}

void lt_texted_erase_selection(lt_texted_t* ed) {
	usz x1, y1, x2, y2;
	if (!lt_texted_get_selection(ed, &x1, &y1, &x2, &y2))
		return;
	lt_texted_erase_range(ed, x1, y1, x2, y2);
	lt_texted_gotoxy(ed, x1, y1, 1);
}

b8 lt_texted_input_str(lt_texted_t* ed, lstr_t str) {
	if (lt_texted_selection_present(ed))
		lt_texted_erase_selection(ed);

	char* it = str.str;
	char* end = str.str + str.len;
	while (it < end) {
		if (*it == '\n') {
			split(ed, ed->cursor_y, ed->cursor_x);
			ed->cursor_x = 0;
			++ed->cursor_y;
			++it;
			continue;
		}
		else if ((*it & 0xC0) == 0x80) {
			++it;
			continue;
		}
		usz len = lt_utf8_decode_len(*it);
		if (it + len > end)
			break;
		lt_darr_insert(ed->lines[ed->cursor_y], ed->cursor_x, it, len);
		ed->cursor_x += len;
		it += len;
	}
	sync_tx(ed);
	sync_s(ed);
	return it != str.str;
}

void lt_texted_cursor_left(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_start(ed);
		return;
	}

	if (ed->cursor_x <= 0) {
		if (ed->cursor_y > 0)
			ed->cursor_x = lt_darr_count(ed->lines[--ed->cursor_y]);
	}
	else {
		// Skip extending UTF-8 bytes
		while ((ed->lines[ed->cursor_y][ed->cursor_x - 1] & 0xC0) == 0x80)
			--ed->cursor_x;
		--ed->cursor_x;
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_cursor_right(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_end(ed);
		return;
	}

	if (ed->cursor_x >= lt_darr_count(ed->lines[ed->cursor_y])) {
		if (ed->cursor_y < lt_darr_count(ed->lines) - 1) {
			++ed->cursor_y;
			ed->cursor_x = 0;
		}
	}
	else {
		ed->cursor_x += lt_utf8_decode_len(ed->lines[ed->cursor_y][ed->cursor_x]);
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_cursor_up(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && ed->cursor_y != ed->select_y) {
		goto_selection_start(ed);
		return;
	}

	if (ed->cursor_y > 0) {
		--ed->cursor_y;
		goto_tx(ed);
	}
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_cursor_down(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && ed->cursor_y != ed->select_y) {
		goto_selection_end(ed);
		return;
	}
	if (ed->cursor_y < lt_darr_count(ed->lines) - 1) {
		++ed->cursor_y;
		goto_tx(ed);
	}
	if (sync_selection)
		sync_s(ed);
}

usz lt_texted_find_word_fwd(lt_texted_t* ed) {
	usz cpos = ed->cursor_x;
	char* str = ed->lines[ed->cursor_y];
	usz len = lt_darr_count(str);

	while (cpos < len && lt_is_space(str[cpos]))
		++cpos;
	if (cpos >= len)
		return cpos;

	char c = str[cpos]; // !! Ignores UTF-8 characters

	if (lt_is_ident_head(c) || lt_is_numeric_head(c)) {
		while (cpos < len && str[cpos] == '_')
			++cpos;
		while (cpos < len && lt_is_numeric_body(str[cpos]))
			cpos += lt_utf8_decode_len(str[cpos]);
	}
	else {
		while (cpos < len && !lt_is_ident_body(str[cpos]) && !lt_is_space(str[cpos]))
			++cpos;
	}

	return cpos;
}

usz lt_texted_find_word_bwd(lt_texted_t* ed) {
	usz cpos = ed->cursor_x;
	char* str = ed->lines[ed->cursor_y];

	while (cpos && lt_is_space(str[cpos - 1]))
		--cpos;
	if (!cpos)
		return 0;

	char c = str[cpos - 1]; // !! Ignores UTF-8 characters

	if (lt_is_ident_head(c) || lt_is_numeric_head(c)) {
		while(cpos && str[cpos - 1] == '_')
			--cpos;
		while(cpos && lt_is_numeric_body(str[cpos - 1])) {
			--cpos;

			// Skip extending UTF-8 bytes
			while ((str[cpos - 1] & 0xC0) == 0x80)
				--cpos;
		}
	}
	else {
		while (cpos && !lt_is_ident_body(str[cpos - 1]) && !lt_is_space(str[cpos - 1]))
			--cpos;
	}
	return cpos;
}

void lt_texted_step_left(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_start(ed);
		return;
	}

	if (ed->cursor_x <= 0) {
		if (ed->cursor_y > 0)
			ed->cursor_x = lt_darr_count(ed->lines[--ed->cursor_y]);
	}
	else {
		ed->cursor_x = lt_texted_find_word_bwd(ed);
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_step_right(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_end(ed);
		return;
	}

	if (ed->cursor_x >= lt_darr_count(ed->lines[ed->cursor_y])) {
		if (ed->cursor_y < lt_darr_count(ed->lines) - 1) {
			++ed->cursor_y;
			ed->cursor_x = 0;
		}
	}
	else {
		ed->cursor_x = lt_texted_find_word_fwd(ed);
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

#define VSTEP 2

void lt_texted_step_up(lt_texted_t* ed, b8 sync_selection) {
	for (usz i = 0; i < VSTEP; ++i)
		lt_texted_cursor_up(ed, sync_selection);
}

void lt_texted_step_down(lt_texted_t* ed, b8 sync_selection) {
	for (usz i = 0; i < VSTEP; ++i)
		lt_texted_cursor_down(ed, sync_selection);
}

void lt_texted_delete_bwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	if (ed->cursor_x <= 0) {
		if (ed->cursor_y > 0) {
			usz posx = lt_darr_count(ed->lines[--ed->cursor_y]);
			ed->cursor_x = posx;
			merge_line(ed, ed->cursor_y);
		}
	}
	else {
		usz start_pos = ed->cursor_x--;
		char* str = ed->lines[ed->cursor_y];
		while (ed->cursor_x > 0 && (str[ed->cursor_x] & 0xC0) == 0x80)
			--ed->cursor_x;

		lt_darr_erase(ed->lines[ed->cursor_y], ed->cursor_x, start_pos - ed->cursor_x);
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_delete_fwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	if (ed->cursor_x >= lt_darr_count(ed->lines[ed->cursor_y])) {
		if (ed->cursor_y < lt_darr_count(ed->lines) - 1)
			merge_line(ed, ed->cursor_y);
	}
	else {
		lt_darr_erase(ed->lines[ed->cursor_y], ed->cursor_x, lt_utf8_decode_len(ed->lines[ed->cursor_y][ed->cursor_x]));
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_delete_word_bwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	if (ed->cursor_x == 0) {
		if (ed->cursor_y > 0) {
			usz posx = lt_darr_count(ed->lines[--ed->cursor_y]);
			ed->cursor_x = posx;
			merge_line(ed, ed->cursor_y);
		}
	}
	else {
		usz word_start = lt_texted_find_word_bwd(ed);
		usz del_len = ed->cursor_x - word_start;
		lt_darr_erase(ed->lines[ed->cursor_y], word_start, del_len);
		ed->cursor_x = word_start;
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_delete_word_fwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	if (ed->cursor_x >= lt_darr_count(ed->lines[ed->cursor_y])) {
		if (ed->cursor_y < lt_darr_count(ed->lines) - 1)
			merge_line(ed, ed->cursor_y);
	}
	else {
		usz del_len = lt_texted_find_word_fwd(ed) - ed->cursor_x;
		lt_darr_erase(ed->lines[ed->cursor_y], ed->cursor_x, del_len);
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_break_line(lt_texted_t* ed) {
	split(ed, ed->cursor_y, ed->cursor_x);
	++ed->cursor_y;
	ed->cursor_x = 0;

	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_gotox(lt_texted_t* ed, usz x, b8 sync_selection) {
	ed->cursor_x = lt_min_usz(x, lt_darr_count(ed->lines[ed->cursor_y]));
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_gotoy(lt_texted_t* ed, usz y, b8 sync_selection) {
	ed->cursor_y = lt_min_usz(y, lt_darr_count(ed->lines) - 1);
	goto_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_gotoxy(lt_texted_t* ed, usz x, usz y, b8 sync_selection) {
	ed->cursor_y = lt_min_usz(y, lt_darr_count(ed->lines) - 1);
	ed->cursor_x = lt_min_usz(x, lt_darr_count(ed->lines[ed->cursor_y]));
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_delete_selection_prefix(lt_texted_t* ed, lstr_t pfx) {
	usz start_y, end_y;
	lt_texted_get_selection(ed, NULL, &start_y, NULL, &end_y);

	for (usz i = start_y; i <= end_y; ++i) {
		lstr_t line = lt_texted_line_str(ed, i);
		if (line.len >= pfx.len && memcmp(pfx.str, line.str, pfx.len) == 0)
			lt_darr_erase(ed->lines[i], 0, pfx.len);
	}

	ed->cursor_x = lt_min_usz(ed->cursor_x, lt_darr_count(ed->lines[ed->cursor_y]));
	ed->select_x = lt_min_usz(ed->select_x, lt_darr_count(ed->lines[ed->select_y]));

	sync_tx(ed);
}

void lt_texted_prefix_selection(lt_texted_t* ed, lstr_t pfx) {
	usz start_y, end_y;
	lt_texted_get_selection(ed, NULL, &start_y, NULL, &end_y);

	for (usz i = start_y; i <= end_y; ++i)
		lt_darr_insert(ed->lines[i], 0, pfx.str, pfx.len);
	ed->cursor_x += pfx.len;
	ed->select_x += pfx.len;

	sync_tx(ed);
}

void lt_texted_prefix_nonempty_selection(lt_texted_t* ed, lstr_t pfx) {
	usz start_y, end_y;
	lt_texted_get_selection(ed, NULL, &start_y, NULL, &end_y);

	for (usz i = start_y; i <= end_y; ++i) {
		if (!lt_darr_count(ed->lines[i]))
			continue;

		if (i == ed->cursor_y)
			ed->cursor_x += pfx.len;
		if (i == ed->select_y)
			ed->select_x += pfx.len;

		lt_darr_insert(ed->lines[i], 0, pfx.str, pfx.len);
	}
}

usz lt_texted_count_line_leading_indent(lt_texted_t* ed, usz line) {
	lstr_t str = lt_texted_line_str(ed, line);
	usz i = 0;
	while (i < str.len && lt_is_space(str.str[i]))
		++i;
	return i;
}

b8 lt_texted_find_next_occurence(lt_texted_t* ed, lstr_t str, usz* out_x, usz* out_y) {
	usz line = ed->cursor_y;
	usz col = ed->cursor_x;
	usz line_count = lt_darr_count(ed->lines);

	if (!str.len)
		return 0;

	for (usz i = line, j = col + 1; i < line_count; ++i) {
		lstr_t line_str = lt_texted_line_str(ed, i);

		// Search every possible offset in the line
		for (; j + str.len <= line_str.len; ++j) {
			if (memcmp(line_str.str + j, str.str, str.len) == 0) {
				if (out_y)
					*out_y = i;
				if (out_x)
					*out_x = j;
				return 1;
			}
		}

		j = 0;
	}

	return 0;
}

b8 lt_texted_find_last_occurence(lt_texted_t* ed, lstr_t str, usz* out_x, usz* out_y) {
	isz line = ed->cursor_y;
	isz col = ed->cursor_x;

	if (!str.len)
		return 0;

	isz i = line, j = col - 1;
	for (;;) {
		lstr_t line_str = lt_texted_line_str(ed, i);

		// Search every possible offset in the line
		for (; j >= (isz)str.len; --j) {
			isz wstart = j - str.len;

			if (memcmp(line_str.str + wstart, str.str, str.len) == 0) {
				if (out_y)
					*out_y = i;
				if (out_x)
					*out_x = wstart;
				return 1;
			}
		}

		if (!(i--))
			break;
		j = lt_darr_count(ed->lines[i]) - 1;
	}

	return 0;
}

b8 lt_texted_iterate_occurences(lt_texted_t* ed, lstr_t str, lt_texted_iterator_t* it) {
	if (!str.len)
		return 0;

	usz line = it->line;
	usz col = it->col + 1;
	usz line_count = lt_darr_count(ed->lines);

	usz i = line, j = col;
	for (; i < line_count; ++i) {
		lstr_t line_str = lt_texted_line_str(ed, i);

		// Search every possible offset in the line
		for (; j + str.len <= line_str.len; ++j) {
			if (memcmp(line_str.str + j, str.str, str.len) == 0) {
				it->line = i;
				it->col = j;
				return 1;
			}
		}

		j = 0;
	}

	it->line = i;
	it->col = j;
	return 0;
}

