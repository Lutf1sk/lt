#pragma once

#include <lt2/common.h>

void add_debug_hooks(err* err);

void set_root_frame_impl(void* root);
#define set_root_frame() set_root_frame_impl(__builtin_frame_address(0))

ls symname_at(void* addr);

void log_ip(log_sink* logger, void* ip);
void log_stack_trace(log_sink* logger, usz skip_frames);

