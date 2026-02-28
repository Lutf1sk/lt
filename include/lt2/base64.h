#pragma once

#include <lt2/common.h>

usz b64_encoded_len(usz size);
usz b64_encode(void* dst, void* src, usz src_size);

usz b64_decoded_len(void* src, usz size, err* error);
usz b64_decode(void* dst, void* src, usz src_size, err* error);

