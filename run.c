#include <lt2/common.h>
#include <lt2/window.h>
#include <lt2/log.h>

#include <lt2/time.h>

u8 key_states[256];
u8 button_states[16];

constexpr f64 move_speed = 200.0;
constexpr f64 boost_mult = 2.0;
constexpr f64 boost_ep_cost = 10.0;

constexpr i32 max_hp = 100;

constexpr i32 max_ep = 20;
constexpr f64 ep_regen = 2.0;
constexpr u64 ep_regen_delay_ns = MS_TO_NS(500);

f64 hp = max_hp;
f64 ep = max_ep;

u64 ep_regen_at_ns;

#ifdef ON_WASI
f64 ui_scale = 1.7;
#else
f64 ui_scale = 1.0;
#endif

void on_frame() {
#ifndef ON_WASI
	sleep_us(1000);
#endif
	fill_rect(0, 0, window_width, window_height, 0xFF1a1a1a);

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

	static u64 prev_time_ns;
	f64 cur_time_ns = time_ns();

	f64 delta = (f64)(cur_time_ns - prev_time_ns) * (1.0 / 1000000000.0);
	prev_time_ns = cur_time_ns;

	static f64 x = 0.0;
	static f64 y = 0.0;

	f64 dx = key_states['D'] - key_states['A'];
	f64 dy = key_states['S'] - key_states['W'];

	f64 inv_mag = 0.0;
	if (dx && dy)
		inv_mag = 0.70707;
	else if (dx || dy)
		inv_mag = 1.0;

	dx *= inv_mag;
	dy *= inv_mag;

	f64 boost = 1.0;
	if ((dx || dy) && key_states[KEY_LSHIFT] && ep > 0) {
		boost = boost_mult;

		ep -= boost_ep_cost * delta;
		ep_regen_at_ns = cur_time_ns + ep_regen_delay_ns;
	}
	else if (cur_time_ns >= ep_regen_at_ns) {
		ep += delta * ep_regen;
	}

	if (hp > max_hp)
		hp = max_hp;
	if (ep > max_ep)
		ep = max_ep;

	x += dx * delta * move_speed * boost;
	y += dy * delta * move_speed * boost;

	fill_circle(x, y, 15, 0xFF00FF00);

#define BAR_HEIGHT 13
#define BAR_WIDTH  200

#define BAR_POSX 10
#define BAR_POSY 10

	f64 hp_fac = (f64)hp / (f64)max_hp;
	f64 ep_fac = (f64)ep / (f64)max_ep;

	fill_rect(BAR_POSX * ui_scale, BAR_POSY * ui_scale, BAR_WIDTH * ui_scale,          BAR_HEIGHT * ui_scale, 0xFF440000);
	fill_rect(BAR_POSX * ui_scale, BAR_POSY * ui_scale, BAR_WIDTH * hp_fac * ui_scale, BAR_HEIGHT * ui_scale, 0xFFFF0000);
	draw_rect(BAR_POSX * ui_scale, BAR_POSY * ui_scale, BAR_WIDTH * ui_scale,          BAR_HEIGHT * ui_scale, 0xFFFFFFFF);

	fill_rect(BAR_POSX * ui_scale, (BAR_POSY + BAR_HEIGHT + 5) * ui_scale, BAR_WIDTH * ui_scale,          BAR_HEIGHT * ui_scale, 0xFF442200);
	fill_rect(BAR_POSX * ui_scale, (BAR_POSY + BAR_HEIGHT + 5) * ui_scale, BAR_WIDTH * ep_fac * ui_scale, BAR_HEIGHT * ui_scale, 0xFFFFEE00);
	draw_rect(BAR_POSX * ui_scale, (BAR_POSY + BAR_HEIGHT + 5) * ui_scale, BAR_WIDTH * ui_scale,          BAR_HEIGHT * ui_scale, 0xFFFFFFFF);

	window_present();
}

#ifdef ON_WASI
void wasi_on_frame() {
	on_frame();
}
#endif

int main(int argc, char** argv) {
	window_init(err_warn);

#ifndef ON_WASI
	while (true)
		on_frame();
#endif
	return 0;
}

