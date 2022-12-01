#include <lt/textedit.h>
#include <lt/darr.h>
#include <lt/math.h>
#include <lt/ctype.h>
#include <lt/utf8.h>

static
usz find_word_bwd(lt_lineedit_t* ed) {
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
usz find_word_fwd(lt_lineedit_t* ed) {
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

b8 lt_lineedit_create(lt_lineedit_t* ed, lt_alloc_t* alloc) {
	ed->cursor_pos = 0;
	ed->str = lt_darr_create(char, 16, alloc);
	return ed->str != NULL;
}

void lt_lineedit_input_str(lt_lineedit_t* ed, lstr_t str) {
	lt_darr_insert(ed->str, ed->cursor_pos, str.str, str.len);
	ed->cursor_pos += str.len;
}

void lt_lineedit_cursor_left(lt_lineedit_t* ed) {
	if (ed->cursor_pos > 0)
		--ed->cursor_pos;
}

void lt_lineedit_cursor_right(lt_lineedit_t* ed) {
	if (ed->cursor_pos < lt_darr_count(ed->str))
		++ed->cursor_pos;
}

void lt_lineedit_step_left(lt_lineedit_t* ed) {
	ed->cursor_pos = find_word_bwd(ed);
}

void lt_lineedit_step_right(lt_lineedit_t* ed) {
	ed->cursor_pos = find_word_fwd(ed);
}

void lt_lineedit_delete_bwd(lt_lineedit_t* ed) {
	if (ed->cursor_pos > 0)
		lt_darr_erase(ed->str, --ed->cursor_pos, 1);
}

void lt_lineedit_delete_fwd(lt_lineedit_t* ed) {
	if (ed->cursor_pos < lt_darr_count(ed->str))
		lt_darr_erase(ed->str, ed->cursor_pos, 1);
}

void lt_lineedit_delete_word_bwd(lt_lineedit_t* ed) {
	usz word_start = find_word_bwd(ed);
	usz del_len = ed->cursor_pos - word_start;
	ed->cursor_pos = word_start;
	lt_darr_erase(ed->str, ed->cursor_pos, del_len);
}

void lt_lineedit_delete_word_fwd(lt_lineedit_t* ed) {
	usz del_len = find_word_fwd(ed) - ed->cursor_pos;
	lt_darr_erase(ed->str, ed->cursor_pos, del_len);
}

void lt_lineedit_gotox(lt_lineedit_t* ed, usz x) {
	if (lt_darr_count(ed->str) <= 0) {
		ed->cursor_pos = 0;
		return;
	}
	ed->cursor_pos = lt_min_usz(lt_darr_count(ed->str), x);
}



static
void merge_bwd(lt_textedit_t* ed) {
	usz dst_idx = ed->cursor_pos - 1;
	usz src_idx = ed->cursor_pos;
	lt_darr_insert(ed->lines[dst_idx].str, lt_darr_count(ed->lines[dst_idx].str), ed->lines[src_idx].str, lt_darr_count(ed->lines[src_idx].str));
	lt_darr_erase(ed->lines, src_idx, 1);
}

static
void merge_fwd(lt_textedit_t* ed) {
	usz dst_idx = ed->cursor_pos;
	usz src_idx = ed->cursor_pos + 1;
	lt_darr_insert(ed->lines[dst_idx].str, lt_darr_count(ed->lines[dst_idx].str), ed->lines[src_idx].str, lt_darr_count(ed->lines[src_idx].str));
	lt_darr_erase(ed->lines, src_idx, 1);
}

static
void sync_tx(lt_textedit_t* ed) {
	ed->target_x = ed->lines[ed->cursor_pos].cursor_pos;
}

b8 lt_textedit_create(lt_textedit_t* ed, lt_alloc_t* alloc) {
	lt_lineedit_t first_line;
	if (!lt_lineedit_create(&first_line, alloc))
		return 0;

	ed->cursor_pos = 0;
	ed->target_x = 0;
	ed->lines = lt_darr_create(lt_lineedit_t, 16, alloc);
	lt_darr_push(ed->lines, first_line);
	return ed->lines != NULL;
}

isz lt_textedit_write_contents(lt_textedit_t* ed, void* usr, lt_io_callback_t callb) {
	isz bytes = 0;
	for (usz i = 0; i < lt_darr_count(ed->lines); ++i) {
		isz res = callb(usr, ed->lines[i].str, lt_darr_count(ed->lines[i].str));
		if (res < 0)
			return res;
		bytes += res;

		res = callb(usr, "\n", 1);
		if (res < 0)
			return res;
		bytes += res;
	}
	return bytes;
}

void lt_textedit_input_str(lt_textedit_t* ed, lstr_t str) {
	lt_lineedit_input_str(&ed->lines[ed->cursor_pos], str);
	sync_tx(ed);
}

void lt_textedit_cursor_left(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos <= 0) {
		if (ed->cursor_pos > 0) {
			--ed->cursor_pos;
			lt_lineedit_gotox(&ed->lines[ed->cursor_pos], -1);
		}
	}
	else {
		lt_lineedit_cursor_left(led);
	}
	sync_tx(ed);
}

void lt_textedit_cursor_right(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 < lt_darr_count(ed->lines)) {
			++ed->cursor_pos;
			lt_lineedit_gotox(&ed->lines[ed->cursor_pos], 0);
		}
	}
	else {
		lt_lineedit_cursor_right(led);
	}
	sync_tx(ed);
}

void lt_textedit_cursor_up(lt_textedit_t* ed) {
	if (ed->cursor_pos > 0) {
		--ed->cursor_pos;
		lt_lineedit_gotox(&ed->lines[ed->cursor_pos], ed->target_x);
	}
}

void lt_textedit_cursor_down(lt_textedit_t* ed) {
	if (ed->cursor_pos + 1 < lt_darr_count(ed->lines)) {
		++ed->cursor_pos;
		lt_lineedit_gotox(&ed->lines[ed->cursor_pos], ed->target_x);
	}
}

void lt_textedit_step_left(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos <= 0) {
		if (ed->cursor_pos > 0) {
			--ed->cursor_pos;
			lt_lineedit_gotox(&ed->lines[ed->cursor_pos], -1);
		}
	}
	else {
		lt_lineedit_step_left(led);
	}
	sync_tx(ed);
}

void lt_textedit_step_right(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 < lt_darr_count(ed->lines)) {
			++ed->cursor_pos;
			lt_lineedit_gotox(&ed->lines[ed->cursor_pos], 0);
		}
	}
	else {
		lt_lineedit_step_right(led);
	}
	sync_tx(ed);
}

#define VSTEP 2

void lt_textedit_step_up(lt_textedit_t* ed) {
	for (usz i = 0; i < VSTEP; ++i)
		lt_textedit_cursor_up(ed);
}

void lt_textedit_step_down(lt_textedit_t* ed) {
	for (usz i = 0; i < VSTEP; ++i)
		lt_textedit_cursor_down(ed);
}

void lt_textedit_delete_bwd(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos == 0) {
		if (ed->cursor_pos <= 0)
			return;
		usz posx = lt_darr_count(ed->lines[ed->cursor_pos - 1].str);
		merge_bwd(ed);
		--ed->cursor_pos;
		lt_lineedit_gotox(&ed->lines[ed->cursor_pos], posx);
	}
	else {
		lt_lineedit_delete_bwd(led);
	}
	sync_tx(ed);
}

void lt_textedit_delete_fwd(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 >= lt_darr_count(ed->lines))
			return;
		merge_fwd(ed);
	}
	else {
		lt_lineedit_delete_fwd(led);
	}
	sync_tx(ed);
}

void lt_textedit_delete_word_bwd(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos == 0) {
		if (ed->cursor_pos <= 0)
			return;
		usz posx = lt_darr_count(ed->lines[ed->cursor_pos - 1].str);
		merge_bwd(ed);
		--ed->cursor_pos;
		lt_lineedit_gotox(&ed->lines[ed->cursor_pos], posx);
	}
	else {
		lt_lineedit_delete_word_bwd(led);
	}
	sync_tx(ed);
}

void lt_textedit_delete_word_fwd(lt_textedit_t* ed) {
	lt_lineedit_t* led = &ed->lines[ed->cursor_pos];
	if (led->cursor_pos >= lt_darr_count(led->str)) {
		if (ed->cursor_pos + 1 >= lt_darr_count(ed->lines))
			return;
		merge_fwd(ed);
	}
	else {
		lt_lineedit_delete_word_fwd(&ed->lines[ed->cursor_pos]);
	}
	sync_tx(ed);
}

void lt_textedit_break_line(lt_textedit_t* ed) {
	usz src_idx = ed->cursor_pos;
	usz dst_idx = ed->cursor_pos + 1;

	lt_lineedit_t new_line;
	LT_ASSERT(lt_lineedit_create(&new_line, lt_darr_head(ed->lines)->alloc));
	lt_darr_insert(ed->lines, dst_idx, &new_line, 1);

	usz move_start_idx = ed->lines[src_idx].cursor_pos;
	char* move_start_src = &ed->lines[src_idx].str[move_start_idx];
	usz move_count = lt_darr_count(ed->lines[src_idx].str) - move_start_idx;
	lt_darr_insert(ed->lines[dst_idx].str, 0, move_start_src, move_count);
	lt_darr_erase(ed->lines[src_idx].str, move_start_idx, move_count);

	ed->cursor_pos = dst_idx;
	ed->lines[dst_idx].cursor_pos = 0;

	sync_tx(ed);
}

void lt_textedit_gotox(lt_textedit_t* ed, usz x) {
	lt_lineedit_gotox(&ed->lines[ed->cursor_pos], x);
	sync_tx(ed);
}

void lt_textedit_gotoy(lt_textedit_t* ed, usz y) {
	// The subtraction is safe because the line count is already guaranteed not to be 0
	ed->cursor_pos = lt_max_usz(y, lt_darr_count(ed->lines) - 1);
}

