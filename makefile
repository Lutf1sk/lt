OUT = lt.a

SRC = \
	src/arg/arg.c \
	src/asm/asm.c \
	src/asm/x64.c \
	src/audio/audio.c \
	src/audio/wav.c \
	src/base64/base64.c \
	src/conf/conf.c \
	src/ctype/ctype.c \
	src/darr/darr.c \
	src/debug/assert.c \
	src/debug/breakpoint.c \
	src/debug/debug.c \
	src/debug/stack_trace.c \
	src/dwarf/dwarf.c \
	src/elf/dwarf.c \
	src/elf/elf.c \
	src/err/err.c \
	src/font/font.c \
	src/font/psf.c \
	src/gfx/gl.c \
	src/gl/gl.c \
	src/gl/glad.c \
	src/gui/gui.c \
	src/hashtab/hashtab.c \
	src/http/http.c \
	src/img/bmp.c \
	src/img/img.c \
	src/img/tga.c \
	src/io/alloc.c \
	src/io/cli.c \
	src/io/file.c \
	src/io/fmt.c \
	src/io/std.c \
	src/io/str.c \
	src/json/json.c \
	src/linalg/matrix.c \
	src/lpc/lex.c \
	src/lpc/parse.c \
	src/lpc/gen.c \
	src/lt/dynl.c \
	src/lt/err.c \
	src/mem/arena.c \
	src/mem/heap.c \
	src/mem/mset.c \
	src/mem/page_size.c \
	src/mem/pool.c \
	src/mem/vmem.c \
	src/net/socket.c \
	src/net/sockstream.c \
	src/shell/shell.c \
	src/spotify/spotify.c \
	src/ssl/ssl.c \
	src/str/str.c \
	src/strstream/strstream.c \
	src/term/term.c \
	src/texted/texted.c \
	src/thread/mutex.c \
	src/thread/spinlock.c \
	src/thread/thread.c \
	src/time/sleep.c \
	src/time/time.c \
	src/utf8/utf8.c \
	src/utf8/wcwidth.c \
	src/vk/vk.c \
	src/window/win32.c \
	src/window/window.c \
	src/window/x11.c \
	src/window/xproto.c

#ifdef WINDOWS
#	CC = x86_64-w64-mingw32-gcc
#	LNK = x86_64-w64-mingw32-gcc
#endif

# -----== COMPILER
CC := cc
CC_WARN := -Wall -Werror -Wno-strict-aliasing -Wno-error=unused-variable -Wno-unused-function -Wno-pedantic
CC_FLAGS := -I./include/ -std=gnu2x -fmax-errors=3 $(CC_WARN) -mavx2 -masm=intel

ifdef DEBUG
	CC_FLAGS += -fno-omit-frame-pointer -O0 -g
else
	CC_FLAGS += -O2
endif

ifdef SSL
	CC_FLAGS += -DLT_SSL=1
endif
ifdef ASOUND
	CC_FLAGS += -DLT_ASOUND=1
endif
ifdef VK
	CC_FLAGS += -DLT_VK=1
endif
ifdef X11
	CC_FLAGS += -DLT_X11=1
endif

# -----== LINKER
LNK := cc
LNK_LIBS := -lpthread -ldl -lm
LNK_FLAGS := -o $(OUT) $(LNK_LIBS)

ifdef DEBUG
	LNK_FLAGS += -g -rdynamic
endif

# -----== TARGETS
ifdef DEBUG
	BIN_PATH := bin/debug
else
	BIN_PATH := bin/release
endif

OUT_PATH := $(BIN_PATH)/$(OUT)

OBJS := $(patsubst %.c,$(BIN_PATH)/%.o,$(SRC))
DEPS := $(patsubst %.o,%.deps,$(OBJS))

all: $(OUT_PATH)

$(OUT_PATH): $(OBJS)
	-mkdir -p bin/
	$(LNK) $(LNK_FLAGS) -r -o $(OUT_PATH) $(OBJS)

install: all
	cp $(OUT_PATH) /usr/local/bin/

clean:
	-rm -r bin

$(BIN_PATH)/%.o: %.c makefile
	@-mkdir -p $(BIN_PATH)/$(dir $<)
	@$(CC) $(CC_FLAGS) -MM -MT $@ -MF $(patsubst %.o,%.deps,$@) $<
	$(CC) $(CC_FLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: all install clean

