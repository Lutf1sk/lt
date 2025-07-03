#pragma once

#include <lt2/common.h>

typedef struct parsed_uri {
	ls scheme;
	ls host;
	ls path;
	ls query;
	u16 port;
	u16 pad[3];
} parsed_uri;

parsed_uri parse_uri(ls uri_str, err* error);

