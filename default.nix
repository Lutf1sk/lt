with import (builtins.fetchTarball {
	url = "https://github.com/NixOS/nixpkgs/archive/refs/tags/24.05.tar.gz";
}) {};

let
	warnings = [ "-Wall" "-Werror" "-Wno-strict-aliasing" "-Wno-error=unused-variable" "-Wno-error=unused-function" "-Wno-error=unused-label" ];

	src = [
		src/cli.c
		src/err.c
		src/fmt.c
		src/fs.c
		src/http.c
		src/http_server.c
		src/ini.c
		src/page.c
		src/posix.c
		src/socket.c
		src/str.c
		src/thread.c
		src/time.c
		src/tls_socket.c
		src/uri.c
		src/vmap.c
		src/mime.c
		src/log.c
	];
in

stdenv.mkDerivation {
	name = "lt2";
	dontUnpack = true;

	privateHeaders = src/posix.h;
	includePath = ./include;
	src = src;
	cflags = warnings ++ [ "-std=gnu2x" "-I${./include}" "-I${./src}" "-fmax-errors=3" ];

	buildPhase = ''
		$CC $cflags -r $src -o lt2.a
	'';
	installPhase = ''
		mkdir -p $out/lib
		cp lt2.a $out/lib/
	'';
}
