#ifndef LT_TEXTEDIT_H
#define LT_TEXTEDIT_H 1

#include <lt/err.h>
#include <lt/fwd.h>
#include <lt/darr.h>
#include <lt/debug.h>

// texted.c

typedef usz (*lt_texted_find_visual_x_fn_t)(void* usr, lstr_t str, usz cursor_x);
typedef usz (*lt_texted_find_cursor_x_fn_t)(void* usr, lstr_t str, usz visual_x);

typedef
struct lt_texted {
	usz cursor_x, cursor_y;
	usz select_x, select_y;
	usz target_x;
	lt_darr(lt_darr(char)) lines;

	void* usr;
	lt_texted_find_visual_x_fn_t find_visual_x;
	lt_texted_find_cursor_x_fn_t find_cursor_x;
} lt_texted_t;

lt_err_t lt_texted_create(lt_texted_t ed[static 1], lt_alloc_t alloc[static 1]);
void lt_texted_destroy(lt_texted_t ed[static 1]);

void lt_texted_clear(lt_texted_t ed[static 1]);

static LT_INLINE
usz lt_texted_line_count(lt_texted_t ed[static 1]) {
	return lt_darr_count(ed->lines);
}

static LT_INLINE
lstr_t lt_texted_line_str(lt_texted_t ed[static 1], usz line) {
	return LSTR(ed->lines[line], lt_darr_count(ed->lines[line]));
}

static LT_INLINE
usz lt_texted_line_len(lt_texted_t ed[static 1], usz line) {
	return lt_darr_count(ed->lines[line]);
}

isz lt_texted_write_contents(lt_texted_t ed[static 1], lt_write_fn_t callb, void* usr);
isz lt_texted_write_range(lt_texted_t ed[static 1], usz x1, usz y1, usz x2, usz y2, lt_write_fn_t callb, void* usr);
void lt_texted_erase_range(lt_texted_t ed[static 1], usz x1, usz y1, usz x2, usz y2);

b8 lt_texted_selection_present(lt_texted_t ed[static 1]);
b8 lt_texted_get_selection(lt_texted_t ed[static 1], usz* out_x1, usz* out_y1, usz* out_x2, usz* out_y2);
isz lt_texted_write_selection(lt_texted_t ed[static 1], lt_write_fn_t callb, void* usr);
usz lt_texted_selection_len(lt_texted_t ed[static 1]);
void lt_texted_erase_selection(lt_texted_t ed[static 1]);

b8 lt_texted_input_str(lt_texted_t ed[static 1], lstr_t str);

void lt_texted_cursor_left(lt_texted_t ed[static 1], b8 sync_selection);
void lt_texted_cursor_right(lt_texted_t ed[static 1], b8 sync_selection);
void lt_texted_cursor_up(lt_texted_t ed[static 1], b8 sync_selection);
void lt_texted_cursor_down(lt_texted_t ed[static 1], b8 sync_selection);

usz lt_texted_find_word_fwd(lt_texted_t ed[static 1]);
usz lt_texted_find_word_bwd(lt_texted_t ed[static 1]);

void lt_texted_step_left(lt_texted_t ed[static 1], b8 sync_selection);
void lt_texted_step_right(lt_texted_t ed[static 1], b8 sync_selection);
void lt_texted_step_up(lt_texted_t ed[static 1], b8 sync_selection);
void lt_texted_step_down(lt_texted_t ed[static 1], b8 sync_selection);

void lt_texted_delete_bwd(lt_texted_t ed[static 1]);
void lt_texted_delete_fwd(lt_texted_t ed[static 1]);
void lt_texted_delete_word_bwd(lt_texted_t ed[static 1]);
void lt_texted_delete_word_fwd(lt_texted_t ed[static 1]);

void lt_texted_break_line(lt_texted_t ed[static 1]);

void lt_texted_gotox(lt_texted_t ed[static 1], usz x, b8 sync_selection);
void lt_texted_gotoy(lt_texted_t ed[static 1], usz y, b8 sync_selection);

void lt_texted_gotoxy(lt_texted_t ed[static 1], usz x, usz y, b8 sync_selection);

void lt_texted_delete_selection_prefix(lt_texted_t ed[static 1], lstr_t pfx);
void lt_texted_prefix_selection(lt_texted_t ed[static 1], lstr_t pfx);
void lt_texted_prefix_nonempty_selection(lt_texted_t ed[static 1], lstr_t pfx);

usz lt_texted_count_line_leading_indent(lt_texted_t ed[static 1], usz line);

b8 lt_texted_find_next_occurence(lt_texted_t ed[static 1], lstr_t str, usz* out_x, usz* out_y);
b8 lt_texted_find_last_occurence(lt_texted_t ed[static 1], lstr_t str, usz* out_x, usz* out_y);

typedef
struct lt_texted_iterator {
	isz line, col;
} lt_texted_iterator_t;

static LT_INLINE
lt_texted_iterator_t lt_texted_iterator_create(void) {
	return (lt_texted_iterator_t){ .line = 0, .col = -1 };
}

b8 lt_texted_iterate_occurences(lt_texted_t ed[static 1], lstr_t str, lt_texted_iterator_t* it);
b8 lt_texted_iterate_occurences_bwd(lt_texted_t ed[static 1], lstr_t str, lt_texted_iterator_t* it);

// cli.c

b8 lt_texted_input_term_key(lt_texted_t ed[static 1], lt_strstream_t* clipboard, u32 key);

#endif
