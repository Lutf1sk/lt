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
typedef struct lt_sockstream lt_sockstream_t;

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

// hashtab.h
typedef struct lt_hashtab lt_hashtab_t;

// strstream.h
typedef struct lt_strstream lt_strstream_t;

// img.h
typedef struct lt_img lt_img_t;

// darr.h
#define lt_darr(T) T*

// gfx.h
typedef struct lt_gfx lt_gfx_t;
typedef struct lt_pipeline lt_pipeline_t;
typedef struct lt_mesh lt_mesh_t;
typedef struct lt_texture lt_texture_t;
typedef struct lt_model lt_model_t;

// textedit.h
typedef struct lt_texted lt_texted_t;

// linalg.h
typedef union lt_vec2 lt_vec2_t;
typedef union lt_vec3 lt_vec3_t;
typedef union lt_vec4 lt_vec4_t;

typedef union lt_mat2 lt_mat2_t;
typedef union lt_mat3 lt_mat3_t;
typedef union lt_mat4 lt_mat4_t;

// elf.h
typedef struct lt_elf64_fh lt_elf64_fh_t;
typedef struct lt_elf32_fh lt_elf32_fh_t;
typedef struct lt_elf64_ph lt_elf64_ph_t;
typedef struct lt_elf32_ph lt_elf32_ph_t;
typedef struct lt_elf64_sh lt_elf64_sh_t;
typedef struct lt_elf32_sh lt_elf32_sh_t;
typedef struct lt_elf64_sym lt_elf64_sym_t;
typedef struct lt_elf32_sym lt_elf32_sym_t;
typedef struct lt_elf64_dyn lt_elf64_dyn_t;
typedef struct lt_elf64 lt_elf64_t;

#endif
