#ifndef LT_TEXTEDIT_H
#define LT_TEXTEDIT_H 1

#include <lt/err.h>
#include <lt/fwd.h>
#include <lt/darr.h>

typedef
struct lt_led {
	usz cursor_pos;
	usz select_pos;
	lt_darr(char) str;
} lt_led_t;

lt_err_t lt_led_create(lt_led_t* ed, lt_alloc_t* alloc);

static LT_INLINE
void lt_led_destroy(lt_led_t* ed) {
	lt_darr_destroy(ed->str);
}

static LT_INLINE
void lt_led_clear(lt_led_t* ed) {
	lt_darr_clear(ed->str);
	ed->cursor_pos = 0;
}

static LT_INLINE
lstr_t lt_led_get_str(lt_led_t* ed) {
	return LSTR(ed->str, lt_darr_count(ed->str));
}

usz lt_led_get_selection(lt_led_t* ed, usz* out_x1, usz* out_x2);

static LT_INLINE
lstr_t lt_led_get_selection_str(lt_led_t* ed) {
	usz x1;
	usz len = lt_led_get_selection(ed, &x1, NULL);
	return LSTR(&ed->str[x1], len);
}

static LT_INLINE
void lt_led_erase_selection(lt_led_t* ed) {
	usz x1;
	usz len = lt_led_get_selection(ed, &x1, NULL);
	lt_darr_erase(ed->str, x1, len);
	ed->select_pos = ed->cursor_pos = x1;
}

b8 lt_led_input_str(lt_led_t* ed, lstr_t str);
void lt_led_cursor_left(lt_led_t* ed, b8 sync_selection);
void lt_led_cursor_right(lt_led_t* ed, b8 sync_selection);
void lt_led_step_left(lt_led_t* ed, b8 sync_selection);
void lt_led_step_right(lt_led_t* ed, b8 sync_selection);

void lt_led_delete_bwd(lt_led_t* ed);
void lt_led_delete_fwd(lt_led_t* ed);
void lt_led_delete_word_bwd(lt_led_t* ed);
void lt_led_delete_word_fwd(lt_led_t* ed);

void lt_led_gotox(lt_led_t* ed, usz x, b8 sync_selection);



typedef
struct lt_texted {
	usz cursor_pos;
	usz select_x, select_y;
	usz target_x;
	lt_darr(lt_led_t) lines;
} lt_texted_t;

lt_err_t lt_texted_create(lt_texted_t* ed, lt_alloc_t* alloc);
void lt_texted_destroy(lt_texted_t* ed);

void lt_texted_clear(lt_texted_t* ed);

isz lt_texted_write_contents(lt_texted_t* ed, void* usr, lt_io_callback_t callb);
isz lt_texted_write_range(lt_texted_t* ed, usz x1, usz y1, usz x2, usz y2, void* usr, lt_io_callback_t callb);
void lt_texted_erase_range(lt_texted_t* ed, usz x1, usz y1, usz x2, usz y2);

static LT_INLINE
void lt_texted_get_cursor(lt_texted_t* ed, usz* out_x, usz* out_y) {
	if (out_y)
		*out_y = ed->cursor_pos;
	if (out_x)
		*out_x = ed->lines[ed->cursor_pos].cursor_pos;
}

b8 lt_texted_selection_present(lt_texted_t* ed);
b8 lt_texted_get_selection(lt_texted_t* ed, usz* out_x1, usz* out_y1, usz* out_x2, usz* out_y2);
isz lt_texted_write_selection(lt_texted_t* ed, void* usr, lt_io_callback_t callb);
void lt_texted_erase_selection(lt_texted_t* ed);

b8 lt_texted_input_str(lt_texted_t* ed, lstr_t str);

void lt_texted_cursor_left(lt_texted_t* ed, b8 sync_selection);
void lt_texted_cursor_right(lt_texted_t* ed, b8 sync_selection);
void lt_texted_cursor_up(lt_texted_t* ed, b8 sync_selection);
void lt_texted_cursor_down(lt_texted_t* ed, b8 sync_selection);

void lt_texted_step_left(lt_texted_t* ed, b8 sync_selection);
void lt_texted_step_right(lt_texted_t* ed, b8 sync_selection);
void lt_texted_step_up(lt_texted_t* ed, b8 sync_selection);
void lt_texted_step_down(lt_texted_t* ed, b8 sync_selection);

void lt_texted_delete_bwd(lt_texted_t* ed);
void lt_texted_delete_fwd(lt_texted_t* ed);
void lt_texted_delete_word_bwd(lt_texted_t* ed);
void lt_texted_delete_word_fwd(lt_texted_t* ed);

void lt_texted_break_line(lt_texted_t* ed);

void lt_texted_gotox(lt_texted_t* ed, usz x, b8 sync_selection);
void lt_texted_gotoy(lt_texted_t* ed, usz y, b8 sync_selection);

static LT_INLINE
void lt_texted_gotoxy(lt_texted_t* ed, usz x, usz y, b8 sync_selection) {
	lt_texted_gotoy(ed, y, sync_selection);
	lt_texted_gotox(ed, x, sync_selection);
}

#endif
