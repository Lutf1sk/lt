OUT = lt.a

SRC = \
	src/cli.c \
	src/err.c \
	src/fmt.c \
	src/fs.c \
	src/http.c \
	src/http_server.c \
	src/ini.c \
	src/page.c \
	src/posix.c \
	src/socket.c \
	src/str.c \
	src/thread.c \
	src/time.c \
	src/tls_socket.c \
	src/uri.c \
	src/vmap.c \
	src/mime.c \
	src/log.c

# -----== COMPILER
CC := cc
CC_WARN := -Wall -Werror -Wno-strict-aliasing -Wno-error=unused-variable -Wno-unused-function -Wno-pedantic -Wno-unused-label
CC_FLAGS := -I./include/ -std=c2x -fmax-errors=3 $(CC_WARN)

ifndef ARM
	CC_FLAGS += -mavx2 -masm=intel
endif

ifdef WINDOWS
	CC = x86_64-w64-mingw32-gcc
endif

ifdef DEBUG
	CC_FLAGS += -fno-omit-frame-pointer -O0 -g -DLT_DEBUG=1
else
	CC_FLAGS += -O2
endif

ifdef OPENSSL
	CC_FLAGS += -DLT_OPENSSL=1
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
LNK_LIBS :=
LNK_FLAGS := -o $(OUT) $(LNK_LIBS)

ifdef WINDOWS
	LNK = x86_64-w64-mingw32-gcc
endif

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
	$(CC) $(CC_FLAGS) -MD -MT $@ -MF $(patsubst %.o,%.deps,$@) -c $< -o $@

-include $(DEPS)

.PHONY: all install clean

