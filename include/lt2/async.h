#pragma once

#include <lt2/common.h>

#define CO_UNIQUE_LABEL EXCAT(__co_label_, __LINE__)

static
task* co_next(task* t) {
	if (t >= t->stack_end) {
		throw(err_fail, ERR_LIMIT_EXCEEDED, "no subtasks available");
		return NULL; // unreachable
	}

	task* next = t + 1;
	*next = (task) {
		.stack_end = t->stack_end
	};
	return next;
}

static
void co_reset(task* t, usz count) {
	for (task* it = t, *end = it + count; it < end; ++it)
		*it = (task) { .stack_end = end };
}

#define co_reenter(t) \
	task* __task = (t); \
	task* co_subtask; \
	(void)co_subtask; \
	do { \
		if (!__task->reenter_at) \
			__task->reenter_at = LABEL_ADDR(CO_UNIQUE_LABEL); \
		void* __jump_addr = __task->reenter_at; \
		__task->reenter_at = NULL; \
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
		co_subtask = co_next(__task); \
	CO_UNIQUE_LABEL:; \
		co_subtask = __task + 1; \
		call; \
		if (co_subtask->running) { \
			__task->reenter_at = LABEL_ADDR(CO_UNIQUE_LABEL); \
			return __VA_ARGS__; \
		}

