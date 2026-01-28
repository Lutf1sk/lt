#include <lt2/common.h>
#include <lt2/time.h>
#include <lt2/log.h>

#include <unistd.h>

int main() {
	log_sink* logger = &(log_sink) {
		.type = LOGSINK_FILE,
		.file = {
			.color = 1,
			.fd    = STDOUT_FILENO,
		},
	};
	logger->next = &(log_sink) {
		.type = LOGSINK_FILE,
		.file = {
			.fd = fcreate(ls("log.txt"), RW, err_warn),
		}
	};

	for (int i = 0; i < 8; ++i)
		llogf(logger, i, "asdf {u32}", 123);

	ls path = ls("test.txt");

	while (1) {
		u32 flags = fwatch_once(path, WATCH_EVENTS, err_warn);
		if (flags & WATCH_FAILED)
			sleep_ms(100);
		if (!(flags & WATCH_EVENTS))
			continue;

		ls str = fmapall(path, RW, err_warn);
		lprintf("content: {ls}\n", str);
	}
}

