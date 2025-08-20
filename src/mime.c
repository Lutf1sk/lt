#include <lt2/str.h>

ls mime_type_or_default(ls path, ls default_) {
#define map_mime(extension, mime) if (lssuffix(path, ls(extension))) return ls(mime)

	map_mime(".aac", "audio/aac");
	map_mime(".abw", "application/x-abiword");
	map_mime(".apng", "image/apng");
	map_mime(".arc", "application/x-freearc");
	map_mime(".avif", "image/avif");
	map_mime(".avi", "video/x-msvideo");
	map_mime(".azw", "application/vnd.amazon.ebook");
	map_mime(".bin", "application/octet-stream");
	map_mime(".bmp", "image/bmp");
	map_mime(".bz", "application/x-bzip");
	map_mime(".bz2", "application/x-bzip2");
	map_mime(".cda", "application/x-cdf");
	map_mime(".conf", "text/plain");
	map_mime(".csh", "application/x-csh");
	map_mime(".css", "text/css");
	map_mime(".csv", "text/csv");
	map_mime(".doc", "application/msword");
	map_mime(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
	map_mime(".eot", "application/vnd.ms-fontobject");
	map_mime(".epub", "application/epub+zip");
	map_mime(".exe", "application/vnd.microsoft.portable-executable");
	map_mime(".flac", "audio/flac");
	map_mime(".gz", "application/gzip");
	map_mime(".gif", "image/gif");
	map_mime(".htm", "text/html");
	map_mime(".html", "text/html");
	map_mime(".ico", "image/vnd.microsoft.icon");
	map_mime(".ics", "text/calendar");
	map_mime(".ini", "text/plain");
	map_mime(".jar", "application/java-archive");
	map_mime(".jpeg", "image/jpeg");
	map_mime(".jpg", "image/jpeg");
	map_mime(".js", "text/javascript");
	map_mime(".json", "application/json");
	map_mime(".jsonld", "application/ld+json");
	map_mime(".log", "text/plain");
	map_mime(".mid", "audio/midi");
	map_mime(".midi", "audio/midi");
	map_mime(".mjs", "text/javascript");
	map_mime(".mkv", "video/x-matroska");
	map_mime(".mov", "video/quicktime");
	map_mime(".mp3", "audio/mp3");
	map_mime(".mp4", "video/mp4");
	map_mime(".mpeg", "video/mpeg");
	map_mime(".mpkg", "application/vnd.apple.installer+xml");
	map_mime(".odp", "aplication/vnd.oasis.opendocument.presentation");
	map_mime(".ods", "aplication/vnd.oasis.opendocument.spreadsheet");
	map_mime(".odt", "aplication/vnd.oasis.opendocument.text");
	map_mime(".oga", "audio/ogg");
	map_mime(".ogg", "audio/ogg");
	map_mime(".ogv", "video/ogg");
	map_mime(".ogx", "application/ogg");
	map_mime(".opus", "audio/opus");
	map_mime(".otf", "font/otf");
	map_mime(".png", "image/png");
	map_mime(".pdf", "application/pdf");
	map_mime(".php", "application/x-httpd-php");
	map_mime(".ppt", "application/vnd.ms-powerpoint");
	map_mime(".pptx", "application/vnd.openxmlformats-officedocuments.presentationml.presentation");
	map_mime(".rar", "application/vnd.rar");
	map_mime(".rtf", "application/rtf");
	map_mime(".sh", "application/x-sh");
	map_mime(".svg", "image/svg+xml");
	map_mime(".tar", "application/x-tar");
	map_mime(".tif", "image/tiff");
	map_mime(".tiff", "image/tiff");
	map_mime(".ts", "video/mp2t");
	map_mime(".ttf", "font/ttf");
	map_mime(".txt", "text/plain");
	map_mime(".vsd", "application/vnd.visio");
	map_mime(".wav", "audio/wav");
	map_mime(".weba", "audio/webm");
	map_mime(".webm", "video/webm");
	map_mime(".webp", "image/webp");
	map_mime(".woff", "font/woff");
	map_mime(".woff2", "font/woff2");
	map_mime(".xhtml", "application/xhtml+xml");
	map_mime(".xls", "application/vnd.ms-excel");
	map_mime(".xlsx", "application/vnd.openxmlformats-officedocuments.spreadsheetml.sheet");
	map_mime(".xml", "application/xml");
	map_mime(".xul", "application/vnd.mozilla.xul+xml");
	map_mime(".zip", "application/zip");
	map_mime(".3gp", "video/3gpp");
	map_mime(".3g2", "video/3gpp2");
	map_mime(".7z", "application/x-7z-compressed");

	// C
	map_mime(".c", "text/plain");
	map_mime(".h", "text/plain");

	// C++
	map_mime(".c++", "text/plain");
	map_mime(".cc", "text/plain");
	map_mime(".cp", "text/plain");
	map_mime(".cpp", "text/plain");
	map_mime(".cppm", "text/plain");
	map_mime(".cxx", "text/plain");
	map_mime(".hh", "text/plain");
	map_mime(".hpp", "text/plain");
	map_mime(".h++", "text/plain");

	// GNU Make
	map_mime("makefile", "text/plain");

	// Git
	map_mime(".gitignore", "text/plain");
	map_mime(".gitmodules", "text/plain");
	map_mime(".gitattributes", "text/plain");

	// LICENSE
	map_mime("LICENSE", "text/plain");

	// Rust
	map_mime(".rs", "text/plain");

	// Zig
	map_mime(".zig", "text/plain");

	// Onyx
	map_mime(".nyx", "text/plain");

	// C#
	map_mime(".cs", "text/plain");
	map_mime(".cshtml", "text/plain");
	map_mime(".c#", "text/plain");
	map_mime(".razor", "text/plain");

	// Haskell
	map_mime(".hs", "text/plain");
	map_mime(".lhs", "text/plain");

	// Python
	map_mime(".py", "text/plain");

	// Editor project files
	map_mime(".sln", "text/plain");
	map_mime(".csproj", "text/plain");

	map_mime(".iso", "application/octet-stream");

	return default_;
}

ls mime_type_from_ext(ls path) {
	return mime_type_or_default(path, ls("application/octet-stream"));
}

