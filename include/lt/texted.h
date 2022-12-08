#ifndef LT_TEXTEDIT_H
#define LT_TEXTEDIT_H 1

#include <lt/err.h>
#include <lt/fwd.h>
#include <lt/darr.h>

typedef
struct lt_led {
	usz cursor_pos;
	lt_darr(char) str;
} lt_led_t;

lt_err_t lt_led_create(lt_led_t* ed, lt_alloc_t* alloc);

static LT_INLINE
void lt_led_destroy(lt_led_t* ed) {
	lt_darr_destroy(ed->str);
}

static LT_INLINE
lstr_t lt_led_getstr(lt_led_t* ed) {
	return LSTR(ed->str, lt_darr_count(ed->str));
}

static LT_INLINE
void lt_led_clear(lt_led_t* ed) {
	lt_darr_clear(ed->str);
	ed->cursor_pos = 0;
}

b8 lt_led_input_str(lt_led_t* ed, lstr_t str);
void lt_led_cursor_left(lt_led_t* ed);
void lt_led_cursor_right(lt_led_t* ed);
void lt_led_step_left(lt_led_t* ed);
void lt_led_step_right(lt_led_t* ed);

void lt_led_delete_bwd(lt_led_t* ed);
void lt_led_delete_fwd(lt_led_t* ed);
void lt_led_delete_word_bwd(lt_led_t* ed);
void lt_led_delete_word_fwd(lt_led_t* ed);

void lt_led_gotox(lt_led_t* ed, usz x);



typedef
struct lt_texted {
	usz cursor_pos;
	usz target_x;
	lt_darr(lt_led_t) lines;
} lt_texted_t;

lt_err_t lt_texted_create(lt_texted_t* ed, lt_alloc_t* alloc);
void lt_texted_destroy(lt_texted_t* ed);

void lt_texted_clear(lt_texted_t* ed);

isz lt_texted_write_contents(lt_texted_t* ed, void* usr, lt_io_callback_t callb);

b8 lt_texted_input_str(lt_texted_t* ed, lstr_t str);

void lt_texted_cursor_left(lt_texted_t* ed);
void lt_texted_cursor_right(lt_texted_t* ed);
void lt_texted_cursor_up(lt_texted_t* ed);
void lt_texted_cursor_down(lt_texted_t* ed);

void lt_texted_step_left(lt_texted_t* ed);
void lt_texted_step_right(lt_texted_t* ed);
void lt_texted_step_up(lt_texted_t* ed);
void lt_texted_step_down(lt_texted_t* ed);

void lt_texted_delete_bwd(lt_texted_t* ed);
void lt_texted_delete_fwd(lt_texted_t* ed);
void lt_texted_delete_word_bwd(lt_texted_t* ed);
void lt_texted_delete_word_fwd(lt_texted_t* ed);

void lt_texted_break_line(lt_texted_t* ed);

void lt_texted_gotox(lt_texted_t* ed, usz x);
void lt_texted_gotoy(lt_texted_t* ed, usz y);

#endif
