#include <lt2/uri.h>
#include <lt2/str.h>

#include <ctype.h>

static parsed_uri parse_uri_pass1(ls uri, err* error) {
	parsed_uri parsed = {0};

	u8* start = uri.ptr;
	u8* end   = start + uri.size;
	u8* it    = start;

	// ----- scheme
	for (;;) {
		if (it >= end) {
			parsed.host = lsrange(start, end);
			return parsed;
		}

		u8 c = *it;
		if (c == ':' && it + 1 < end && it[1] == '/')
			break;
		if (!isalnum(c) && c != '+' && c != '-' && c != '.')
			goto host;
		++it;
	}

	if (end - it < 2 || it[1] != '/' || it[2] != '/') {
		throw(error, ERR_ANY, "expected '://' after url scheme");
		return parsed;
	}
	parsed.scheme = lsrange(start, it);
	start = (it += 3);

	// ----- host
host:
	for (;;) {
		if (it >= end) {
			parsed.host = lsrange(start, end);
			return parsed;
		}

		u8 c = *it;
		if (c == '/' || c == '?' || c == '#') {
			parsed.host = lsrange(start, it);
			break;
		}
		++it;
	}
	start = it;

	// ----- path
	for (;;) {
		if (it >= end) {
			parsed.path = lsrange(start, end);
			return parsed;
		}

		u8 c = *it;
		if (c == '?' || c == '#') {
			parsed.path = lsrange(start, it);
			break;
		}
		++it;
	}

	// ----- query
	parsed.query = lsrange(it, end);
	return parsed;
}

parsed_uri parse_uri(ls uri_str, err* error) {
	parsed_uri uri = parse_uri_pass1(uri_str, error);
	if (lseq(uri.scheme, ls("http")))
		uri.port = 80;
	if (lseq(uri.scheme, ls("https")))
		uri.port = 443;

	u8* hostname_end = lssubstr(uri.host, ls(":"));
	if (hostname_end) {
		u8* host_end = uri.host.ptr + uri.host.size;
		uri.host = lsrange(uri.host.ptr, hostname_end);
		uri.port = lstou(lsrange(hostname_end + 1, host_end), err_warn);
	}

	return uri;
}


