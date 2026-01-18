#include <lt2/common.h>
#include <lt2/async.h>
#include <lt2/time.h>

typedef struct main_async_state {
	u64 start_ms;
} main_async_state;

void main_async(task* t, main_async_state* state) {
	co_reenter(t);

	state->start_ms = time_ms();
	while (time_ms() - state->start_ms < 1000)
		co_yield();
	lprintf("wait completed\n");

	co_await(main_async(co_subtask, state));
}

int main() {
	struct main_async_state main_state = {0};

	task tasks[16] = {0};
	tasks->stack_end = tasks + 16;
	do {
		main_async(tasks, &main_state);
	} while (tasks->running);
}
