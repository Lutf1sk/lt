#ifndef LT_TEXTEDIT_H
#define LT_TEXTEDIT_H 1

#include <lt/lt.h>
#include <lt/fwd.h>
#include <lt/darr.h>

typedef
struct lt_lineedit {
	usz cursor_pos;
	lt_darr(char) str;
} lt_lineedit_t;

b8 lt_lineedit_create(lt_lineedit_t* ed, lt_alloc_t* alloc);

static LT_INLINE
void lt_lineedit_destroy(lt_lineedit_t* ed) {
	lt_darr_destroy(ed->str);
}

static LT_INLINE
lstr_t lt_lineedit_getstr(lt_lineedit_t* ed) {
	return LSTR(ed->str, lt_darr_count(ed->str));
}

static LT_INLINE
void lt_lineedit_clear(lt_lineedit_t* ed) {
	lt_darr_clear(ed->str);
	ed->cursor_pos = 0;
}

b8 lt_lineedit_input_str(lt_lineedit_t* ed, lstr_t str);
void lt_lineedit_cursor_left(lt_lineedit_t* ed);
void lt_lineedit_cursor_right(lt_lineedit_t* ed);
void lt_lineedit_step_left(lt_lineedit_t* ed);
void lt_lineedit_step_right(lt_lineedit_t* ed);

void lt_lineedit_delete_bwd(lt_lineedit_t* ed);
void lt_lineedit_delete_fwd(lt_lineedit_t* ed);
void lt_lineedit_delete_word_bwd(lt_lineedit_t* ed);
void lt_lineedit_delete_word_fwd(lt_lineedit_t* ed);

void lt_lineedit_gotox(lt_lineedit_t* ed, usz x);



typedef
struct lt_textedit {
	usz cursor_pos;
	usz target_x;
	lt_darr(lt_lineedit_t) lines;
} lt_textedit_t;

b8 lt_textedit_create(lt_textedit_t* ed, lt_alloc_t* alloc);
void lt_textedit_destroy(lt_textedit_t* ed);

void lt_textedit_clear(lt_textedit_t* ed);

isz lt_textedit_write_contents(lt_textedit_t* ed, void* usr, lt_io_callback_t callb);

b8 lt_textedit_input_str(lt_textedit_t* ed, lstr_t str);

void lt_textedit_cursor_left(lt_textedit_t* ed);
void lt_textedit_cursor_right(lt_textedit_t* ed);
void lt_textedit_cursor_up(lt_textedit_t* ed);
void lt_textedit_cursor_down(lt_textedit_t* ed);

void lt_textedit_step_left(lt_textedit_t* ed);
void lt_textedit_step_right(lt_textedit_t* ed);
void lt_textedit_step_up(lt_textedit_t* ed);
void lt_textedit_step_down(lt_textedit_t* ed);

void lt_textedit_delete_bwd(lt_textedit_t* ed);
void lt_textedit_delete_fwd(lt_textedit_t* ed);
void lt_textedit_delete_word_bwd(lt_textedit_t* ed);
void lt_textedit_delete_word_fwd(lt_textedit_t* ed);

void lt_textedit_break_line(lt_textedit_t* ed);

void lt_textedit_gotox(lt_textedit_t* ed, usz x);
void lt_textedit_gotoy(lt_textedit_t* ed, usz y);

#endif
