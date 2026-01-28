#include <lt2/common.h>
#include <lt2/time.h>
#include <lt2/log.h>

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

