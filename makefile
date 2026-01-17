
HEADERS = $(wildcard include/lt2/*.h)
SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,bin/obj/%.o,$(SRC))

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

bin/run: bin/lt2.a run.c
	$(CC) $(CFLAGS) run.c -lcrypto -lssl -lpthread -lm bin/lt2.a -o bin/run

bin/lt2.a: $(OBJ)
	ar -rcs bin/lt2.a $(OBJ)

bin/obj/%.o: src/%.c $(HEADERS) makefile
	@-mkdir -p bin/obj
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all run clean

