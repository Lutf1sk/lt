#include <lt2/common.h>

task* co_next(task* t) {
	if (t + 1 >= t->stack_end) {
		throw(err_fail, ERR_LIMIT_EXCEEDED, "no subtasks available");
		return NULL; // unreachable
	}

	task* next = t + 1;
	*next = (task) {
		.stack_end = t->stack_end
	};
	return next;
}

void co_reset(task* t, usz count) {
	for (task* it = t, *end = it + count; it < end; ++it)
		*it = (task) { .stack_end = end };
}


#ifdef ON_LINUX

#	include <poll.h>
#	include <unistd.h>

u8 poll_handle(file_handle fd, u8 mode, u64 timeout_ms) {
	i16 poll_mode = POLLERR;
	if (mode & R)
		poll_mode |= POLLIN;
	if (mode & W)
		poll_mode |= POLLOUT;

	struct pollfd pfd = {
		.fd     = fd,
		.events = poll_mode
	};

	if (timeout_ms > INT32_MAX)
		timeout_ms = INT32_MAX;

	if (poll(&pfd, 1, timeout_ms) < 0)
		return E;

	u8 res = 0;
	if (pfd.revents & POLLERR)
		res |= E;
	if (pfd.revents & POLLIN)
		res |= R;
	if (pfd.revents & POLLOUT)
		res |= W;
	return res & mode;
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

#endif // ON_LINUX

