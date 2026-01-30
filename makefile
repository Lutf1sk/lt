
HEADERS = $(wildcard include/lt2/*.h)
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,bin/obj/%.o,$(SRC))

OUT = bin/lt.a
RUN = bin/run

CC = cc
CFLAGS = \
	-I./include \
	-O0 -g -std=gnu2x \
	-Wall -Werror

ifdef WASI
CC = clang
CFLAGS += -DON_WASI=1 --target=wasm32
OUT = bin/lt2.wasm
RUN = bin/run.wasm
OBJ = $(patsubst src/%.c,bin/obj/%.wasm,$(SRC))
endif

ifdef WAYLAND
HEADERS += include/lt2/wayland/xdg-shell-client.h
SRC     += src/wayland/xdg-shell.c
endif

all: $(OUT)

ifndef WASI
run: $(RUN)
	-bin/run $(args)
else
run: $(RUN)
endif

clean:
	-rm -r bin/
	-rm -r include/lt2/wayland/
	-rm -r src/wayland/

src/wayland/xdg-shell.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	@-mkdir -p $(dir $@)
	wayland-scanner private-code <$< >$@

include/lt2/wayland/xdg-shell-client.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	@-mkdir -p $(dir $@)
	wayland-scanner client-header <$< >$@

bin/run: bin/lt2.a run.c
	$(CC) $(CFLAGS) run.c bin/lt2.a -o bin/run

bin/run.wasm: bin/lt2.wasm run.c
	$(CC) -nostdlib -Wl,--no-entry,--export-all $(CFLAGS) $(OBJ) run.c -o bin/run.wasm

bin/lt2.a: $(OBJ)
	ar -rcs bin/lt2.a $(OBJ)

bin/lt2.wasm: $(OBJ)
	#$(CC) $(CFLAGS) $(OBJ) -o bin/lt2.wasm

bin/obj/%.o: src/%.c $(HEADERS) makefile
	@-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

bin/obj/%.wasm: src/%.c $(HEADERS) makefile
	@-mkdir -p $(dir $@)
	$(CC) -nostdlib $(CFLAGS) -c $< -o $@

.PHONY: all run clean

