#ifndef LT_FWD_H
#define LT_FWD_H 1

// io.h
typedef struct lt_file lt_file_t;
typedef struct lt_dir lt_dir_t;
typedef enum lt_dirent_type lt_dirent_type_t;
typedef struct lt_dirent lt_dirent_t;

// mem.h
typedef struct lt_alloc lt_alloc_t;
typedef struct lt_arena lt_arena_t;
typedef struct lt_pool lt_pool_t;

// net.h
typedef struct lt_socket lt_socket_t;
typedef struct lt_sockaddr lt_sockaddr_t;
typedef struct lt_sockstream lt_sockstream_t;

// window.h
typedef struct lt_window lt_window_t;

// thread.h
typedef struct lt_thread lt_thread_t;
typedef struct lt_mutex lt_mutex_t;
typedef struct lt_semaphore lt_semaphore_t;

// arg.h
typedef struct lt_arg_iterator lt_arg_iterator_t;

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
typedef struct lt_led lt_led_t;
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

// audio.h
typedef struct lt_audio_stream lt_audio_stream_t;
typedef struct lt_audio lt_audio_t;

// ssl.h
typedef struct lt_ssl_connection lt_ssl_connection_t;

// http.h
typedef struct lt_http_msg lt_http_msg_t;

// lpc.h
typedef struct lt_lpc_expr lt_lpc_expr_t;
typedef struct lt_lpc_stmt lt_lpc_stmt_t;
typedef struct lt_lpc_tk lt_lpc_tk_t;
typedef struct lt_lpc_parse_ctx lt_lpc_parse_ctx_t;
typedef struct lt_lpc_lex_ctx lt_lpc_lex_ctx_t;

// c.h
typedef struct lt_c_type lt_c_type_t;
typedef struct lt_c_expr lt_c_expr_t;
typedef struct lt_c_stmt lt_c_stmt_t;
typedef struct lt_c_tk lt_c_tk_t;
typedef struct lt_c_parse_ctx lt_c_parse_ctx_t;
typedef struct lt_c_define lt_c_define_t;
typedef struct lt_c_preproc_template lt_c_preproc_template_t;
typedef struct lt_c_preproc_scope lt_c_preproc_scope_t;
typedef struct lt_c_preproc_ctx lt_c_preproc_ctx_t;
typedef struct lt_c_sym lt_c_sym_t;
typedef struct lt_c_symtab lt_c_symtab_t;

#endif
