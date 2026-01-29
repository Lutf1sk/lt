#include <lt2/common.h>
#include <lt2/time.h>
#include <lt2/log.h>

typedef struct arena {
	u8* top;
	u8* base;
	u8* end;
} arena_t;

INLINE
void* arena_alloc(arena_t* arena, size_t size) {
    uint8_t* top = arena->top;
    uint8_t* new_top = top + ((size + 31) & -32);
    if UNLIKELY (new_top > arena->end)
        return NULL;
    arena->top = new_top;
    return top;
}

INLINE
void* arena_alloc_unsafe(arena_t* arena, size_t size) {
    u8* top = arena->base;
    arena->top += (size + 31) & -32;
    return top;
}

int main() {
	log_sink* logger = &(log_sink) {
		.type = LOGSINK_FILE,
		.file = {
			.color = 1,
			.fd    = 2
		}
	};
	logger->next = &(log_sink) {
		.type = LOGSINK_FILE,
		.file = {
			.fd = fcreate(ls("log.txt"), RW, err_warn)
		}
	};
	default_log_sink = logger;

	void* arena_base = NULL;
	usz   arena_size = 0;

	vmap_t memmap[] = {
		{ .permit = RW, .size = MB(2), .guard_size = 1, .out = &arena_base, .out_size = &arena_size },
	};
	vmap(memmap, COUNT_OF(memmap), 0, err_fail);

	arena_t* arena = &(arena_t) {
		.top  = arena_base,
		.base = arena_base,
		.end  = (u8*)arena_base + arena_size,
	};

	arena_alloc(arena, 128);

	for (int i = 0; i < 8; ++i)
		llogf(NULL, i, "asdf {u32}", 123);

	ls watch_path = ls("test.txt");

	while (1) {
		llogf(NULL, LOG_INFO, "watching '{ls}'", watch_path);
		u32 flags = fwatch_once(watch_path, WATCH_EVENTS, err_warn);
		if (flags & WATCH_FAILED)
			sleep_s(1);
		if (!(flags & WATCH_EVENTS))
			continue;

		ls str = fmapall(watch_path, RW, err_warn);
		lprintf("content: {ls}\n", str);
	}
}

