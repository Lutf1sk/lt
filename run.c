#include <lt2/common.h>
#include <lt2/time.h>
#include <lt2/log.h>

void* arena_alloc(uint8_t** arena, size_t size) {
    void* top = *arena;
    *arena += (uintptr_t)top + ((size + 31) & -32);
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

