#include <lt2/common.h>

#include <poll.h>
#include <unistd.h>

b8 poll_handle(file_handle fd, u8 mode, u64 timeout_ms) {
	i16 poll_mode = 0;
	if (mode & R)
		poll_mode |= POLLIN;
	if (mode & W)
		poll_mode |= POLLOUT;

	struct pollfd pfd = {
		.fd     = fd,
		.events = POLLERR | poll_mode
	};

	if (poll(&pfd, 1, timeout_ms) < 0)
		return 0;
	return !!(pfd.revents & poll_mode);
}

b8 poll_callable(task* t, u64 timeout_ms) {
	task* end = t->stack_end;
	while (t < end && t->running) {
		if (!t->mode) {
			t = t + 1;
			continue;
		}

		return poll_handle(t->fd, t->mode, timeout_ms);
	}
	return 1;
}

