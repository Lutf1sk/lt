LIB = bin/lt.a

OBJS = \
	src/arg/arg.o \
	src/asm/asm.o \
	src/asm/x64.o \
	src/base64/base64.o \
	src/conf/conf.o \
	src/ctype/ctype.o \
	src/darr/darr.o \
	src/debug/assert.o \
	src/debug/breakpoint.o \
	src/debug/debug.o \
	src/debug/stack_trace.o \
	src/dwarf/dwarf.o \
	src/elf/dwarf.o \
	src/elf/elf.o \
	src/err/err.o \
	src/font/font.o \
	src/font/psf.o \
	src/gfx/gl.o \
	src/gl/gl.o \
	src/gl/glad.o \
	src/gui/gui.o \
	src/hashtab/hashtab.o \
	src/http/http.o \
	src/img/bmp.o \
	src/img/img.o \
	src/img/tga.o \
	src/io/alloc.o \
	src/io/cli.o \
	src/io/file.o \
	src/io/fmt.o \
	src/io/std.o \
	src/io/str.o \
	src/json/json.o \
	src/linalg/matrix.o \
	src/lt/dynl.o \
	src/lt/err.o \
	src/mem/arena.o \
	src/mem/heap.o \
	src/mem/mset.o \
	src/mem/page_size.o \
	src/mem/pool.o \
	src/mem/vmem.o \
	src/net/socket.o \
	src/net/sockstream.o \
	src/ssl/ssl.o \
	src/str/str.o \
	src/strstream/strstream.o \
	src/term/term.o \
	src/texted/texted.o \
	src/thread/mutex.o \
	src/thread/spinlock.o \
	src/thread/thread.o \
	src/time/sleep.o \
	src/time/time.o \
	src/utf8/utf8.o \
	src/utf8/wcwidth.o \
	src/vk/vk.o \
	src/vk/volk.o \
	src/window/win32.o \
	src/window/window.o \
	src/window/x11.o \
	src/window/xproto.o

CC = gcc
LNK = gcc

ifdef WINDOWS
	CC = x86_64-w64-mingw32-gcc
	LNK = x86_64-w64-mingw32-gcc
else
	CC_FLAGS += -g -rdynamic
	LNK_FLAGS += -g -rdynamic
endif

ifdef SSL
	CC_FLAGS += -DLT_SSL=1
endif

ifdef DEBUG
	CC_FLAGS += -O0 -g -rdynamic -fno-omit-frame-pointer
else
	CC_FLAGS += -Ofast
endif

# -----
DEPS = $(patsubst %.o,%.d,$(OBJS))

CC_FLAGS += -mavx2 -Wall -I./ -I./include/ -masm=intel -Wall -Werror -Wno-strict-aliasing -Wno-error=unused-variable -Wno-unused-function -std=gnu2x -fmax-errors=3
LNK_FLAGS += -L$(BASE_DIR)/bin

OUT_PATH = $(LIB)

all: $(OUT_PATH)

$(OUT_PATH): $(OBJS)
	@-mkdir -p bin/
	@echo Linking $(LIB)...
	@$(LNK) $(LNK_FLAGS) -r -o $(OUT_PATH) $(OBJS)

.PHONY: clean all run analyze

clean:
	-rm $(OBJS) $(DEPS) $(OUT_PATH)

analyze:
	@clang-check --analyze --extra-arg="-I./include" $(patsubst %.o,%.c,$(OBJS))

%.o: %.c makefile
	@echo Compiling $<...
	@$(CC) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $< $(CC_FLAGS)
	@$(CC) -c $< -o $@ $(CC_FLAGS)

-include $(DEPS)

