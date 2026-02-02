#include <lt2/window.h>

i32 window_height;
i32 window_width;

f64 mouse_x;
f64 mouse_y;

#define MAX_EVENTS 32
#define EVENT_MASK (MAX_EVENTS - 1)

static wevent_t wev_queue[MAX_EVENTS];
static usz wev_first;
static usz wev_count;

void queue_wevent(wevent_t event) {
	if (wev_count >= MAX_EVENTS)
		wev_first = (wev_first + 1) & EVENT_MASK;
	wev_queue[(wev_first + wev_count++) & EVENT_MASK] = event;
}

void platform_poll_wevents();

usz poll_wevents(wevent_t* out, usz max_events) {
	platform_poll_wevents();

	usz count = wev_count;
	if (count > max_events)
		count = max_events;

	for (usz i = 0; i < count; ++i) {
		*out++ = wev_queue[wev_first];
		wev_first = (wev_first + 1) & (MAX_EVENTS - 1);

		--max_events;
		--wev_count;
	}
	return count;
}

