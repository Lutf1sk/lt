#include <lt2/common.h>
#include <lt2/window.h>
#include <lt2/log.h>

#include <lt2/time.h>

u8 key_states[256];
u8 button_states[16];

void on_frame() {
	wevent_t events[16];
	usz event_count = poll_wevents(events, COUNT_OF(events));

	for (wevent_t* ev = events, *end = ev + event_count; ev < end; ++ev) {
		switch (ev->type) {
		case WEV_MOTION:
			break;

		case WEV_BUTTON_PRESS:
			button_states[ev->button.code] = 1;
			break;

		case WEV_BUTTON_RELEASE:
			button_states[ev->button.code] = 0;
			break;

		case WEV_KEY_PRESS:
			key_states[ev->key.code] = 1;
			break;

		case WEV_KEY_RELEASE:
			key_states[ev->key.code] = 0;
			break;

		case WEV_QUIT:
			llogf(NULL, LOG_INFO, "quit");
			exit(0);
			break;

		case WEV_RESIZE:
			llogf(NULL, LOG_INFO, "resize");
			break;
		}
	}

	static int x = 0;
	static int y = 0;

	x += key_states['D'] - key_states['A'];
	y += key_states['S'] - key_states['W'];

#ifndef ON_WASI
	sleep_us(100);
#endif

	fill_rect(0, 0, window_width, window_height, 0xFF080808);

	for (i32 i = -1; i < window_width; i += 100)
		draw_vline(i, 0, window_height, 0xFFFFFFFF);
	for (i32 i = 0; i < window_height; i += 100)
		draw_hline(0, window_height - i, window_width, 0xFFFFFFFF);

	window_present();
}

#ifdef ON_WASI
void wasi_on_frame() {
	on_frame();
}
#endif

int main() {
	window_init(err_warn);

#ifndef ON_WASI
	while (true)
		on_frame();
#endif
	return 0;
}

