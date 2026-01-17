
HEADERS = $(wildcard include/lt2/*.h)
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,bin/obj/%.o,$(SRC))

CC = cc
CFLAGS = \
	-O2 -std=gnu2x \
	-Werror

all: bin/lt.a

clean:
	-rm -r bin/

bin/lt.a: $(OBJ)
	ar -rcs bin/lt.a $(OBJ)

bin/obj/%.o: src/%.c $(HEADERS) makefile
	@-mkdir -p bin/obj
	$(CC) $(CFLAGS) -I./include -c $< -o $@

.PHONY: all clean

