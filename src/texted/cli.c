#include <lt/term.h>
#include <lt/strstream.h>
#include <lt/text.h>
#include <lt/texted.h>
#include <lt/ctype.h>

b8 lt_texted_input_term_key(lt_texted_t* ed, lt_strstream_t* clipboard, u32 key) {
	switch (key) {
	case LT_TERM_KEY_LEFT: lt_texted_cursor_left(ed, 1); return 0;
	case LT_TERM_KEY_RIGHT: lt_texted_cursor_right(ed, 1); return 0;
	case LT_TERM_KEY_LEFT|LT_TERM_MOD_CTRL: lt_texted_step_left(ed, 1); return 0;
	case LT_TERM_KEY_RIGHT|LT_TERM_MOD_CTRL: lt_texted_step_right(ed, 1); return 0;
	case LT_TERM_KEY_UP: lt_texted_cursor_up(ed, 1); return 0;
	case LT_TERM_KEY_DOWN: lt_texted_cursor_down(ed, 1); return 0;
	case LT_TERM_KEY_UP|LT_TERM_MOD_CTRL: lt_texted_step_up(ed, 1); return 0;
	case LT_TERM_KEY_DOWN|LT_TERM_MOD_CTRL: lt_texted_step_down(ed, 1); return 0;

	case LT_TERM_KEY_LEFT|LT_TERM_MOD_SHIFT: lt_texted_cursor_left(ed, 0); return 0;
	case LT_TERM_KEY_RIGHT|LT_TERM_MOD_SHIFT: lt_texted_cursor_right(ed, 0); return 0;
	case LT_TERM_KEY_LEFT|LT_TERM_MOD_CTRL|LT_TERM_MOD_SHIFT: lt_texted_step_left(ed, 0); return 0;
	case LT_TERM_KEY_RIGHT|LT_TERM_MOD_CTRL|LT_TERM_MOD_SHIFT: lt_texted_step_right(ed, 0); return 0;
	case LT_TERM_KEY_UP|LT_TERM_MOD_SHIFT: lt_texted_cursor_up(ed, 0); return 0;
	case LT_TERM_KEY_DOWN|LT_TERM_MOD_SHIFT: lt_texted_cursor_down(ed, 0); return 0;
	case LT_TERM_KEY_UP|LT_TERM_MOD_CTRL|LT_TERM_MOD_SHIFT: lt_texted_step_up(ed, 0); return 0;
	case LT_TERM_KEY_DOWN|LT_TERM_MOD_CTRL|LT_TERM_MOD_SHIFT: lt_texted_step_down(ed, 0); return 0;

	case LT_TERM_KEY_HOME: lt_texted_gotox(ed, 0, 1); return 0;
	case LT_TERM_KEY_HOME|LT_TERM_MOD_SHIFT: lt_texted_gotox(ed, 0, 0); return 0;
	case LT_TERM_KEY_END: lt_texted_gotox(ed, -1, 1); return 0;
	case LT_TERM_KEY_END|LT_TERM_MOD_SHIFT: lt_texted_gotox(ed, -1, 0); return 0;
	case LT_TERM_KEY_HOME|LT_TERM_MOD_CTRL: lt_texted_gotoxy(ed, 0, 0, 1); return 0;
	case LT_TERM_KEY_HOME|LT_TERM_MOD_CTRL|LT_TERM_MOD_SHIFT: lt_texted_gotoxy(ed, 0, 0, 0); return 0;
	case LT_TERM_KEY_END|LT_TERM_MOD_CTRL: lt_texted_gotoxy(ed, -1, -1, 1); return 0;
	case LT_TERM_KEY_END|LT_TERM_MOD_CTRL|LT_TERM_MOD_SHIFT: lt_texted_gotoxy(ed, -1, -1, 0); return 0;

	case LT_TERM_KEY_BSPACE: lt_texted_delete_bwd(ed); return 1;
	case LT_TERM_KEY_DELETE: lt_texted_delete_fwd(ed); return 1;
	case LT_TERM_KEY_BSPACE|LT_TERM_MOD_CTRL: lt_texted_delete_word_bwd(ed); return 1;
	case LT_TERM_KEY_DELETE|LT_TERM_MOD_CTRL: lt_texted_delete_word_fwd(ed); return 1;

	case 'X'|LT_TERM_MOD_CTRL:
		if (!lt_texted_selection_present(ed))
			return 0;
		if (clipboard != NULL) {
			lt_strstream_clear(clipboard);
			lt_texted_write_selection(ed, (lt_io_callback_t)lt_strstream_write, clipboard);
		}
		lt_texted_erase_selection(ed);
		return 1;

	case 'C'|LT_TERM_MOD_CTRL:
		if (!lt_texted_selection_present(ed))
			return 0;
		if (clipboard == NULL)
			return 0;
		lt_strstream_clear(clipboard);
		lt_texted_write_selection(ed, (lt_io_callback_t)lt_strstream_write, clipboard);
		return 1;

	case 'V'|LT_TERM_MOD_CTRL:
		if (clipboard == NULL)
			return 0;
		lt_texted_input_str(ed, clipboard->str);
		return 1;

	case 'D'|LT_TERM_MOD_CTRL:
		lt_texted_gotox(ed, 0, 1);
		lt_texted_gotox(ed, -1, 0);
		return 0;

	case '\n':
		lt_texted_break_line(ed);
		return 1;

	default: {
		if (lt_is_unicode_control_char(key) || (key & (LT_TERM_KEY_SPECIAL_BIT | LT_TERM_MOD_MASK)))
			return 0;

		char utf8_buf[4];
		lstr_t utf8_str = LSTR(utf8_buf, lt_utf8_encode(key, utf8_buf));
		return lt_texted_input_str(ed, utf8_str);
	}
	}
}
