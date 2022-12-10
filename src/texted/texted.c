#include <lt/texted.h>
#include <lt/darr.h>
#include <lt/math.h>
#include <lt/ctype.h>
#include <lt/utf8.h>

static
usz find_word_bwd(lt_led_t* ed) {
	usz cpos = ed->cursor_pos;
	char* str = ed->str;

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

static
usz find_word_fwd(lt_led_t* ed) {
	usz cpos = ed->cursor_pos;
	char* str = ed->str;
	usz len = lt_darr_count(ed->str);

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

lt_err_t lt_led_create(lt_led_t* ed, lt_alloc_t* alloc) {
	ed->cursor_pos = 0;
	ed->select_pos = 0;
	ed->str = lt_darr_create(char, 32, alloc);
	if (!ed->str)
		return LT_ERR_OUT_OF_MEMORY;
	return LT_SUCCESS;
}

usz lt_led_get_selection(lt_led_t* ed, usz* out_x1, usz* out_x2) {
	usz min = ed->cursor_pos;
	usz max = ed->select_pos;

	if (ed->select_pos < ed->cursor_pos) {
		min = ed->select_pos;
		max = ed->cursor_pos;
	}

	if (out_x1)
		*out_x1 = min;
	if (out_x2)
		*out_x2 = max;

	return max - min;
}

b8 lt_led_input_str(lt_led_t* ed, lstr_t str) {
	if (lt_led_get_selection(ed, NULL, NULL))
		lt_led_erase_selection(ed);

	char* it = str.str;
	char* end = str.str + str.len;
	b8 changed = 0;
	while (it < end) {
		if ((u8)*it < 0x20) { // Ignore control characters
			++it;
			continue;
		}
		usz len = lt_utf8_decode_len(*it);
		lt_darr_insert(ed->str, ed->cursor_pos, it, len);
		ed->cursor_pos += len;
		it += len;
		changed = 1;
	}
	ed->select_pos = ed->cursor_pos;
	return changed;
}

void lt_led_cursor_left(lt_led_t* ed, b8 sync_selection) {
	usz x1;
	if (sync_selection && lt_led_get_selection(ed, &x1, NULL)) {
		ed->select_pos = ed->cursor_pos = x1;
		return;
	}

	if (ed->cursor_pos > 0) {
		--ed->cursor_pos;
		// Skip extending UTF-8 bytes
		while (ed->cursor_pos > 0 && (ed->str[ed->cursor_pos] & 0xC0) == 0x80)
			--ed->cursor_pos;
	}
	if (sync_selection)
		ed->select_pos = ed->cursor_pos;
}

void lt_led_cursor_right(lt_led_t* ed, b8 sync_selection) {
	usz x2;
	if (sync_selection && lt_led_get_selection(ed, NULL, &x2)) {
		ed->select_pos = ed->cursor_pos = x2;
		return;
	}

	if (ed->cursor_pos < lt_darr_count(ed->str))
		lt_led_gotox(ed, ed->cursor_pos + lt_utf8_decode_len(ed->str[ed->cursor_pos]), sync_selection);
}

void lt_led_step_left(lt_led_t* ed, b8 sync_selection) {
	usz x1;
	if (sync_selection && lt_led_get_selection(ed, &x1, NULL)) {
		ed->select_pos = ed->cursor_pos = x1;
		return;
	}

	ed->cursor_pos = find_word_bwd(ed);
	if (sync_selection)
		ed->select_pos = ed->cursor_pos;
}

void lt_led_step_right(lt_led_t* ed, b8 sync_selection) {
	usz x2;
	if (sync_selection && lt_led_get_selection(ed, NULL, &x2)) {
		ed->select_pos = ed->cursor_pos = x2;
		return;
	}

	ed->cursor_pos = find_word_fwd(ed);
	if (sync_selection)
		ed->select_pos = ed->cursor_pos;
}

void lt_led_delete_bwd(lt_led_t* ed) {
	if (lt_led_get_selection(ed, NULL, NULL)) {
		lt_led_erase_selection(ed);
		return;
	}

	if (ed->cursor_pos > 0) {
		usz start_pos = ed->cursor_pos--;
		while (ed->cursor_pos > 0 && (ed->str[ed->cursor_pos] & 0xC0) == 0x80)
			--ed->cursor_pos;

		lt_darr_erase(ed->str, ed->cursor_pos, start_pos - ed->cursor_pos);
	}
	ed->select_pos = ed->cursor_pos;
}

void lt_led_delete_fwd(lt_led_t* ed) {
	if (lt_led_get_selection(ed, NULL, NULL)) {
		lt_led_erase_selection(ed);
		return;
	}

	if (ed->cursor_pos < lt_darr_count(ed->str))
		lt_darr_erase(ed->str, ed->cursor_pos, lt_utf8_decode_len(ed->str[ed->cursor_pos]));
	ed->select_pos = ed->cursor_pos;
}

void lt_led_delete_word_bwd(lt_led_t* ed) {
	if (lt_led_get_selection(ed, NULL, NULL)) {
		lt_led_erase_selection(ed);
		return;
	}

	usz word_start = find_word_bwd(ed);
	usz del_len = ed->cursor_pos - word_start;
	ed->cursor_pos = word_start;
	lt_darr_erase(ed->str, ed->cursor_pos, del_len);
	ed->select_pos = ed->cursor_pos;
}

void lt_led_delete_word_fwd(lt_led_t* ed) {
	if (lt_led_get_selection(ed, NULL, NULL)) {
		lt_led_erase_selection(ed);
		return;
	}

	usz del_len = find_word_fwd(ed) - ed->cursor_pos;
	lt_darr_erase(ed->str, ed->cursor_pos, del_len);
	ed->select_pos = ed->cursor_pos;
}

void lt_led_gotox(lt_led_t* ed, usz x, b8 sync_selection) {
	if (lt_darr_count(ed->str) <= 0) {
		ed->cursor_pos = 0;
		return;
	}
	ed->cursor_pos = lt_min_usz(lt_darr_count(ed->str), x);
	if (sync_selection)
		ed->select_pos = ed->cursor_pos;
}



static
void merge_bwd(lt_texted_t* ed) {
	usz dst_idx = ed->cursor_pos - 1;
	usz src_idx = ed->cursor_pos;
	lt_darr_insert(ed->lines[dst_idx].str, lt_darr_count(ed->lines[dst_idx].str), ed->lines[src_idx].str, lt_darr_count(ed->lines[src_idx].str));
	lt_led_destroy(&ed->lines[src_idx]);
	lt_darr_erase(ed->lines, src_idx, 1);
}

static
void merge_fwd(lt_texted_t* ed) {
	usz dst_idx = ed->cursor_pos;
	usz src_idx = ed->cursor_pos + 1;
	lt_darr_insert(ed->lines[dst_idx].str, lt_darr_count(ed->lines[dst_idx].str), ed->lines[src_idx].str, lt_darr_count(ed->lines[src_idx].str));
	lt_led_destroy(&ed->lines[src_idx]);
	lt_darr_erase(ed->lines, src_idx, 1);
}

static
void newline(lt_texted_t* ed, usz posy) {
	lt_led_t new_line;
	LT_ASSERT(lt_led_create(&new_line, lt_darr_head(ed->lines)->alloc) == LT_SUCCESS);
	lt_darr_insert(ed->lines, posy, &new_line, 1);
}

static
void split(lt_texted_t* ed, usz posy, usz posx) {
	usz src_idx = posy;
	usz dst_idx = posy + 1;

	newline(ed, dst_idx);

	char* move_start_src = &ed->lines[src_idx].str[posx];
	usz move_count = lt_darr_count(ed->lines[src_idx].str) - posx;
	lt_darr_insert(ed->lines[dst_idx].str, 0, move_start_src, move_count);
	lt_darr_erase(ed->lines[src_idx].str, posx, move_count);
}

static
void sync_tx(lt_texted_t* ed) {
	ed->target_x = ed->lines[ed->cursor_pos].cursor_pos;
}

static
void sync_s(lt_texted_t* ed) {
	ed->select_y = ed->cursor_pos;
	ed->select_x = ed->lines[ed->cursor_pos].cursor_pos;
}

static
void goto_selection_start(lt_texted_t* ed) {
	usz x1, y1;
	lt_texted_get_selection(ed, &x1, &y1, NULL, NULL);
	ed->cursor_pos = y1;
	lt_led_gotox(&ed->lines[ed->cursor_pos], x1, 1);
}

static
void goto_selection_end(lt_texted_t* ed) {
	usz x2, y2;
	lt_texted_get_selection(ed, NULL, NULL, &x2, &y2);
	ed->cursor_pos = y2;
	lt_led_gotox(&ed->lines[ed->cursor_pos], x2, 1);
}

lt_err_t lt_texted_create(lt_texted_t* ed, lt_alloc_t* alloc) {
	lt_err_t err;

	lt_led_t first_line;
	if ((err = lt_led_create(&first_line, alloc)))
		return err;

	ed->select_y = ed->cursor_pos = 0;
	ed->select_x = ed->target_x = 0;
	ed->lines = lt_darr_create(lt_led_t, 32, alloc);
	if (!ed->lines) {
		lt_led_destroy(&first_line);
		return LT_ERR_OUT_OF_MEMORY;
	}

	lt_darr_push(ed->lines, first_line);
	return LT_SUCCESS;
}

void lt_texted_destroy(lt_texted_t* ed) {
	for (usz i = 0; i < lt_darr_count(ed->lines); ++i)
		lt_darr_destroy(ed->lines[i].str);
	lt_darr_destroy(ed->lines);
}

void lt_texted_clear(lt_texted_t* ed) {
	for (usz i = 1; i < lt_darr_count(ed->lines); ++i)
		lt_darr_destroy(ed->lines[i].str);
	lt_led_clear(&ed->lines[0]);
	ed->cursor_pos = 0;
	sync_s(ed);
	sync_tx(ed);
}

isz lt_texted_write_contents(lt_texted_t* ed, void* usr, lt_io_callback_t callb) {
	isz bytes = callb(usr, ed->lines[0].str, lt_darr_count(ed->lines[0].str));
	if (bytes < 0)
		return bytes;

	for (usz i = 1; i < lt_darr_count(ed->lines); ++i) {
		isz res = callb(usr, "\n", 1);
		if (res < 0)
			return res;
		bytes += res;

		res = callb(usr, ed->lines[i].str, lt_darr_count(ed->lines[i].str));
		if (res < 0)
			return res;
		bytes += res;

	}

	return bytes;
}

isz lt_texted_write_range(lt_texted_t* ed, usz x1, usz y1, usz x2, usz y2, void* usr, lt_io_callback_t callb) {
	LT_ASSERT(y1 >= 0 && y1 < lt_darr_count(ed->lines));
	LT_ASSERT(y2 >= 0 && y2 < lt_darr_count(ed->lines));
	LT_ASSERT(x1 >= 0 && x1 <= lt_darr_count(ed->lines[y1].str));
	LT_ASSERT(x2 >= 0 && x2 <= lt_darr_count(ed->lines[y2].str));

	if (y2 == y1)
		return callb(usr, ed->lines[y1].str + x1, x2 - x1);

	isz bytes = callb(usr, ed->lines[y1].str + x1, lt_darr_count(ed->lines[y1].str) - x1);
	if (bytes < 0)
		return bytes;

	isz res = callb(usr, "\n", 1);
	if (res < 0)
		return res;
	bytes += res;

	usz first_covered_y = y1 + 1;
	for (usz y = first_covered_y; y < y2; ++y) {
		if ((res = callb(usr, ed->lines[y].str, lt_darr_count(ed->lines[y].str))) < 0)
			return res;
		bytes += res;

		if ((res = callb(usr, "\n", 1)) < 0)
			return res;
		bytes += res;
	}

	if ((res = callb(usr, ed->lines[y2].str, lt_darr_count(ed->lines[y2].str) - x2)) < 0)
		return res;
	bytes += res;

	return bytes;
}

void lt_texted_erase_range(lt_texted_t* ed, usz x1, usz y1, usz x2, usz y2) {
	ed->cursor_pos = y1;
	lt_led_gotox(&ed->lines[ed->cursor_pos], x1, 1);
	sync_s(ed);
	sync_tx(ed);

	if (y2 == y1) {
		lt_darr_erase(ed->lines[y1].str, x1, x2 - x1);
		return;
	}

	usz first_covered_y = y1 + 1;
	for (usz y = first_covered_y; y < y2; ++y)
		lt_led_destroy(&ed->lines[y]);

	lt_darr_erase(ed->lines[y1].str, x1, lt_darr_count(ed->lines[y1].str) - x1);
	lt_darr_erase(ed->lines[y2].str, 0, x2);

	lt_darr_erase(ed->lines, first_covered_y, y2 - first_covered_y);

	merge_fwd(ed);
}

b8 lt_texted_selection_present(lt_texted_t* ed) {
	return !(ed->select_y == ed->cursor_pos && ed->select_x == ed->lines[ed->cursor_pos].cursor_pos);
}

b8 lt_texted_get_selection(lt_texted_t* ed, usz* out_x1, usz* out_y1, usz* out_x2, usz* out_y2) {
	usz y1 = ed->cursor_pos;
	usz x1 = ed->lines[ed->cursor_pos].cursor_pos;
	usz y2 = ed->select_y;
	usz x2 = ed->select_x;

	if (y1 > y2 || (y1 == y2 && x1 > x2)) {
		y2 = ed->cursor_pos;
		x2 = ed->lines[ed->cursor_pos].cursor_pos;
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

isz lt_texted_write_selection(lt_texted_t* ed, void* usr, lt_io_callback_t callb) {
	usz x1, y1, x2, y2;
	if (!lt_texted_get_selection(ed, &x1, &y1, &x2, &y2))
		return 0;
	return lt_texted_write_range(ed, x1, y1, x2, y2, usr, callb);
}

void lt_texted_erase_selection(lt_texted_t* ed) {
	usz x1, y1, x2, y2;
	if (!lt_texted_get_selection(ed, &x1, &y1, &x2, &y2))
		return;
	lt_texted_erase_range(ed, x1, y1, x2, y2);
}

b8 lt_texted_input_str(lt_texted_t* ed, lstr_t str) {
	if (lt_texted_selection_present(ed))
		lt_texted_erase_selection(ed);

	char* it = str.str;
	char* end = str.str + str.len;
	b8 changed = 0;
	while (it < end) {
		if (*it == '\n') {
			split(ed, ed->cursor_pos, ed->lines[ed->cursor_pos].cursor_pos);
			lt_led_gotox(&ed->lines[++ed->cursor_pos], 0, 1);
			++it;
			continue;
		}
		usz len = lt_utf8_decode_len(*it);
		lt_led_input_str(&ed->lines[ed->cursor_pos], LSTR(it, len));
		it += len;
		changed = 1;
	}
	sync_tx(ed);
	sync_s(ed);
	return changed;
}

void lt_texted_cursor_left(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_start(ed);
		sync_s(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos <= 0) {
		if (ed->cursor_pos > 0) {
			--ed->cursor_pos;
			lt_led_gotox(&ed->lines[ed->cursor_pos], -1, 1);
		}
	}
	else {
		lt_led_cursor_left(led, 1);
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_cursor_right(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_end(ed);
		sync_s(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 < lt_darr_count(ed->lines)) {
			++ed->cursor_pos;
			lt_led_gotox(&ed->lines[ed->cursor_pos], 0, 1);
		}
	}
	else {
		lt_led_cursor_right(led, 1);
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_cursor_up(lt_texted_t* ed, b8 sync_selection) {
	if (ed->cursor_pos > 0) {
		--ed->cursor_pos;
		lt_led_gotox(&ed->lines[ed->cursor_pos], ed->target_x, 1);
	}
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_cursor_down(lt_texted_t* ed, b8 sync_selection) {
	if (ed->cursor_pos + 1 < lt_darr_count(ed->lines)) {
		++ed->cursor_pos;
		lt_led_gotox(&ed->lines[ed->cursor_pos], ed->target_x, 1);
	}
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_step_left(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_start(ed);
		sync_s(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos <= 0) {
		if (ed->cursor_pos > 0) {
			--ed->cursor_pos;
			lt_led_gotox(&ed->lines[ed->cursor_pos], -1, 1);
		}
	}
	else {
		lt_led_step_left(led, 1);
	}
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_step_right(lt_texted_t* ed, b8 sync_selection) {
	if (sync_selection && lt_texted_selection_present(ed)) {
		goto_selection_end(ed);
		sync_s(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 < lt_darr_count(ed->lines)) {
			++ed->cursor_pos;
			lt_led_gotox(&ed->lines[ed->cursor_pos], 0, 1);
		}
	}
	else {
		lt_led_step_right(led, 1);
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

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos == 0) {
		if (ed->cursor_pos <= 0)
			return;
		usz posx = lt_darr_count(ed->lines[ed->cursor_pos - 1].str);
		merge_bwd(ed);
		--ed->cursor_pos;
		lt_led_gotox(&ed->lines[ed->cursor_pos], posx, 1);
	}
	else {
		lt_led_delete_bwd(led);
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_delete_fwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 >= lt_darr_count(ed->lines))
			return;
		merge_fwd(ed);
	}
	else {
		lt_led_delete_fwd(led);
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_delete_word_bwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos == 0) {
		if (ed->cursor_pos <= 0)
			return;
		usz posx = lt_darr_count(ed->lines[ed->cursor_pos - 1].str);
		merge_bwd(ed);
		--ed->cursor_pos;
		lt_led_gotox(&ed->lines[ed->cursor_pos], posx, 1);
	}
	else {
		lt_led_delete_word_bwd(led);
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_delete_word_fwd(lt_texted_t* ed) {
	if (lt_texted_selection_present(ed)) {
		lt_texted_erase_selection(ed);
		return;
	}

	lt_led_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 >= lt_darr_count(ed->lines))
			return;
		merge_fwd(ed);
	}
	else {
		lt_led_delete_word_fwd(&ed->lines[ed->cursor_pos]);
	}
	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_break_line(lt_texted_t* ed) {
	split(ed, ed->cursor_pos, ed->lines[ed->cursor_pos].cursor_pos);
	lt_led_gotox(&ed->lines[++ed->cursor_pos], 0, 1);

	sync_tx(ed);
	sync_s(ed);
}

void lt_texted_gotox(lt_texted_t* ed, usz x, b8 sync_selection) {
	lt_led_gotox(&ed->lines[ed->cursor_pos], x, 1);
	sync_tx(ed);
	if (sync_selection)
		sync_s(ed);
}

void lt_texted_gotoy(lt_texted_t* ed, usz y, b8 sync_selection) {
	// The subtraction is safe because the line count is already guaranteed not to be 0
	ed->cursor_pos = lt_min_usz(y, lt_darr_count(ed->lines) - 1);
	lt_led_gotox(&ed->lines[ed->cursor_pos], ed->target_x, 1);
	if (sync_selection)
		sync_s(ed);
}

