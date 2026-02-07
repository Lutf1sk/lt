
HEADERS = $(wildcard include/lt2/*.h)
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,bin/obj/%.o,$(SRC))

OUT = bin/lt.a
RUN = bin/run
RUNCMD = -$(RUN) $(args)

CC = cc
CFLAGS = \
	-I./include \
	-std=gnu2x \
	-Wall -Werror -Wno-unused-function -Wno-frame-address
LDFLAGS =


ifdef DEBUG
CFLAGS  += -O0 -g -DDEBUG -fno-optimize-sibling-calls -fno-omit-frame-pointer
LDFLAGS += -O0 -g -rdynamic
else
CFLAGS  += -O2
LDFLAGS += -O2
endif

ifdef ASAN
CFLAGS += -fsanitize=address
endif

ifdef UBSAN
CFLAGS += -fsanitize=undefined
endif


ifdef WASI
RUN = bin/run.wasm
RUNCMD = python -m http.server

CC = clang
CFLAGS  += --target=wasm32
LDFLAGS += -nostdlib -Wl,--export-all
endif

ifdef WAYLAND
HEADERS += include/lt2/wayland/xdg-shell-client.h
SRC     += src/wayland/xdg-shell.c
CFLAGS  += -DWAYLAND `pkg-config --cflags wayland-client`
LDFLAGS += `pkg-config --libs wayland-client`
endif

ifdef OPENSSL
CFLAGS  += -DLT_OPENSSL `pkg-config --cflags openssl`
LDFLAGS += `pkg-config --libs openssl`
endif

all: $(OUT)

run: $(RUN)
	$(RUNCMD)

clean:
	-rm -r bin/
	-rm -r include/lt2/wayland/
	-rm -r src/wayland/

test: all
	@-mkdir -p bin/
	$(CC) $(CFLAGS) test.c $(OUT) -o bin/test $(LDFLAGS)
	bin/test $(args)

src/wayland/xdg-shell.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	@-mkdir -p $(dir $@)
	wayland-scanner private-code <$< >$@

include/lt2/wayland/xdg-shell-client.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	@-mkdir -p $(dir $@)
	wayland-scanner client-header <$< >$@


$(RUN): $(OUT) run.c
	@-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) run.c $(OUT) -o $(RUN) $(LDFLAGS)

$(OUT): $(OBJ)
	@-mkdir -p $(dir $@)
	ar -rcs $(OUT) $(OBJ)

bin/obj/%.o: src/%.c $(HEADERS) makefile
	@-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: all run clean test

