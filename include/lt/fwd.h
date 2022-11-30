#ifndef LT_FWD_H
#define LT_FWD_H 1

// io.h
typedef struct lt_file lt_file_t;

// mem.h
typedef struct lt_alloc lt_alloc_t;
typedef struct lt_arena lt_arena_t;
typedef struct lt_pool lt_pool_t;

// net.h
typedef struct lt_socket lt_socket_t;

// window.h
typedef struct lt_window lt_window_t;

// thread.h
typedef struct lt_thread lt_thread_t;
typedef struct lt_mutex lt_mutex_t;

// arg.h
typedef struct lt_arg lt_arg_t;

// conf.h
typedef struct lt_conf lt_conf_t;

// json.h
typedef struct lt_json lt_json_t;

// font.h
typedef struct lt_font lt_font_t;

// gui.h
typedef struct lt_gui_ctx lt_gui_ctx_t;
typedef struct lt_gui_rect lt_gui_rect_t;
typedef struct lt_gui_point lt_gui_point_t;

// hashtab.h
typedef struct lt_hashtab lt_hashtab_t;

// strstream.h
typedef struct lt_strstream lt_strstream_t;

// darr.h
#define lt_darr(T) T*

#endif
