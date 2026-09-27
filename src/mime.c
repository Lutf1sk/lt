#include <lt2/str.h>

ls mime_type_or_default(ls path, ls default_) {
#define map_mime(extension, mime) if (lssuffix_upper(path, ls(extension))) return ls(mime)

	map_mime(".AAC", "audio/aac");
	map_mime(".ABW", "application/x-abiword");
	map_mime(".APNG", "image/apng");
	map_mime(".ARC", "application/x-freearc");
	map_mime(".AVIF", "image/avif");
	map_mime(".AVI", "video/x-msvideo");
	map_mime(".AZW", "application/vnd.amazon.ebook");
	map_mime(".BIN", "application/octet-stream");
	map_mime(".BMP", "image/bmp");
	map_mime(".BZ", "application/x-bzip");
	map_mime(".BZ2", "application/x-bzip2");
	map_mime(".CDA", "application/x-cdf");
	map_mime(".CONF", "text/plain");
	map_mime(".CSH", "application/x-csh");
	map_mime(".CSS", "text/css");
	map_mime(".CSV", "text/csv");
	map_mime(".DOC", "application/msword");
	map_mime(".DOCX", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	map_mime(".EOT", "application/vnd.ms-fontobject");
	map_mime(".EPUB", "application/epub+zip");
	map_mime(".EXE", "application/vnd.microsoft.portable-executable");
	map_mime(".FLAC", "audio/flac");
	map_mime(".GZ", "application/gzip");
	map_mime(".GIF", "image/gif");
	map_mime(".HTM", "text/html");
	map_mime(".HTML", "text/html");
	map_mime(".ICO", "image/vnd.microsoft.icon");
	map_mime(".ICS", "text/calendar");
	map_mime(".INI", "text/plain");
	map_mime(".JAR", "application/java-archive");
	map_mime(".JPEG", "image/jpeg");
	map_mime(".JPG", "image/jpeg");
	map_mime(".JS", "text/javascript");
	map_mime(".JSON", "application/json");
	map_mime(".JSONLD", "application/ld+json");
	map_mime(".LOG", "text/plain");
	map_mime(".MID", "audio/midi");
	map_mime(".MIDI", "audio/midi");
	map_mime(".MJS", "text/javascript");
	map_mime(".MKV", "video/x-matroska");
	map_mime(".MOV", "video/quicktime");
	map_mime(".MP3", "audio/mp3");
	map_mime(".MP4", "video/mp4");
	map_mime(".MPEG", "video/mpeg");
	map_mime(".MPKG", "application/vnd.apple.installer+xml");
	map_mime(".ODP", "application/vnd.oasis.opendocument.presentation");
	map_mime(".ODS", "application/vnd.oasis.opendocument.spreadsheet");
	map_mime(".ODT", "application/vnd.oasis.opendocument.text");
	map_mime(".OGA", "audio/ogg");
	map_mime(".OGG", "audio/ogg");
	map_mime(".OGV", "video/ogg");
	map_mime(".OGX", "application/ogg");
	map_mime(".OPUS", "audio/opus");
	map_mime(".OTF", "font/otf");
	map_mime(".PNG", "image/png");
	map_mime(".PDF", "application/pdf");
	map_mime(".PHP", "application/x-httpd-php");
	map_mime(".PPT", "application/vnd.ms-powerpoint");
	map_mime(".PPTX", "application/vnd.openxmlformats-officedocuments.presentationml.presentation");
	map_mime(".RAR", "application/vnd.rar");
	map_mime(".RTF", "application/rtf");
	map_mime(".SH", "application/x-sh");
	map_mime(".SVG", "image/svg+xml");
	map_mime(".TAR", "application/x-tar");
	map_mime(".TIF", "image/tiff");
	map_mime(".TIFF", "image/tiff");
	map_mime(".TS", "video/mp2t");
	map_mime(".TTF", "font/ttf");
	map_mime(".TXT", "text/plain");
	map_mime(".VSD", "application/vnd.visio");
	map_mime(".WAV", "audio/wav");
	map_mime(".WEBA", "audio/webm");
	map_mime(".WEBM", "video/webm");
	map_mime(".WEBP", "image/webp");
	map_mime(".WOFF", "font/woff");
	map_mime(".WOFF2", "font/woff2");
	map_mime(".XHTML", "application/xhtml+xml");
	map_mime(".XLS", "application/vnd.ms-excel");
	map_mime(".XLSX", "application/vnd.openxmlformats-officedocuments.spreadsheetml.sheet");
	map_mime(".XML", "application/xml");
	map_mime(".XUL", "application/vnd.mozilla.xul+xml");
	map_mime(".ZIP", "application/zip");
	map_mime(".3GP", "video/3gpp");
	map_mime(".3G2", "video/3gpp2");
	map_mime(".7Z", "application/x-7z-compressed");

	// C
	map_mime(".C", "text/plain");
	map_mime(".H", "text/plain");

	// C++
	map_mime(".C++", "text/plain");
	map_mime(".CC", "text/plain");
	map_mime(".CP", "text/plain");
	map_mime(".CPP", "text/plain");
	map_mime(".CPPM", "text/plain");
	map_mime(".CXX", "text/plain");
	map_mime(".HH", "text/plain");
	map_mime(".HPP", "text/plain");
	map_mime(".H++", "text/plain");

	// GNU Make
	map_mime("MAKEFILE", "text/plain");

	// Git
	map_mime(".GITIGNORE", "text/plain");
	map_mime(".GITMODULES", "text/plain");
	map_mime(".GITATTRIBUTES", "text/plain");

	// LICENSE
	map_mime("LICENSE", "text/plain");

	// Rust
	map_mime(".RS", "text/plain");

	// Zig
	map_mime(".ZIG", "text/plain");

	// Onyx
	map_mime(".NYX", "text/plain");

	// C#
	map_mime(".CS", "text/plain");
	map_mime(".CSHTML", "text/plain");
	map_mime(".C#", "text/plain");
	map_mime(".RAZOR", "text/plain");

	// Haskell
	map_mime(".HS", "text/plain");
	map_mime(".LHS", "text/plain");

	// Python
	map_mime(".PY", "text/plain");

	// Editor project files
	map_mime(".SLN", "text/plain");
	map_mime(".CSPROJ", "text/plain");

	map_mime(".ISO", "application/octet-stream");

	return default_;
}

ls mime_type_from_ext(ls path) {
	return mime_type_or_default(path, ls("application/octet-stream"));
}

