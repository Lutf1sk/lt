#pragma once

#include <lt2/common.h>

#define CO_UNIQUE_LABEL EXCAT(__co_label_, __LINE__)

task* co_next(task* t);
void co_reset(task* t, usz count);

u8 poll_callable(task* t, u64 timeout_ms);

#define co_reenter(t) \
	task* __task = (t); \
	task* co_subtask; \
	(void)co_subtask; \
	do { \
		if (!__task->reenter_at) \
			__task->reenter_at = LABEL_ADDR(CO_UNIQUE_LABEL); \
		void* __jump_addr = __task->reenter_at; \
		__task->reenter_at = NULL; \
		__task->mode       = 0; \
		GOTO_ADDR(__jump_addr); \
	CO_UNIQUE_LABEL:; \
	} while (0)

#define co_yield(...) \
	do { \
		__task->reenter_at = LABEL_ADDR(CO_UNIQUE_LABEL); \
		return __VA_ARGS__; \
		CO_UNIQUE_LABEL:; \
	} while (0)

#define co_await(call, ...) \
	do { \
		co_subtask = co_next(__task); \
	CO_UNIQUE_LABEL:; \
		co_subtask = __task + 1; \
		call; \
		if (co_subtask->running) { \
			__task->reenter_at = LABEL_ADDR(CO_UNIQUE_LABEL); \
			return __VA_ARGS__; \
		} \
	} while (0)

#define co_set_awaiting(__fd, __mode) \
	do { \
		__task->fd = __fd; \
		__task->mode = __mode; \
	} while (0)

