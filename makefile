
HEADERS = $(wildcard include/lt2/*.h)
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,bin/obj/%.o,$(SRC))

ifndef NO_WAYLAND
HEADERS += include/lt2/wayland/xdg-shell-client.h
SRC     += src/wayland/xdg-shell.c
endif

CC = cc
CFLAGS = \
	-I./include \
	-O2 -std=gnu2x \
	-Wall -Werror

all: bin/lt2.a

run: bin/run
	-bin/run $(args)

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
	$(CC) $(CFLAGS) run.c -lcrypto -lssl -lpthread -lm -lwayland-client bin/lt2.a -o bin/run

bin/lt2.a: $(OBJ)
	ar -rcs bin/lt2.a $(OBJ)

bin/obj/%.o: src/%.c $(HEADERS) makefile
	@-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all run clean

