
#include <lt2/common.h>
#include <lt2/async.h>
#include <lt2/time.h>

#include <wayland-client.h>
#include <lt2/wayland/xdg-shell-client.h>

#include <string.h>

#define _POSIX_C_SOURCE 200112L
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

typedef struct window_state {
	struct wl_surface*   wl_surface;
	struct xdg_surface*  xdg_surface;
	struct xdg_toplevel* xdg_toplevel;
	struct wl_buffer*    buffer;
	u32* buffer_data;
} window_state;

// ---

struct wl_shm*        shm          = NULL;
struct wl_compositor* compositor   = NULL;
struct xdg_wm_base*   xdg_wm_base  = NULL;


void xdg_wm_base_ping(void* userdata, struct xdg_wm_base* xdg_wm_base, u32 serial) {
	xdg_wm_base_pong(xdg_wm_base, serial);
}

struct xdg_wm_base_listener xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

void xdg_surface_configure(void* userdata, struct xdg_surface* xdg_surface, u32 serial) {
	window_state* win = userdata;
	xdg_surface_ack_configure(xdg_surface, serial);

	const u32 w = 800, h = 600;
	const usz size = w * h * sizeof(u32);

	const char* path = "/lt2-wl-shm";
	int fd = shm_open(path, O_RDWR | O_CREAT | O_EXCL, 0x0600);
	if (fd < 0) {
		throw_errno(err_fail);
		return;
	}
	shm_unlink(path);

	if (ftruncate(fd, size) < 0) {
		throw_errno(err_fail);
		return;
	}

	u32* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		throw_errno(err_fail);
		return;
	}

	struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
	struct wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, w, h, w * sizeof(u32), WL_SHM_FORMAT_XRGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	win->buffer = buffer;
	win->buffer_data = data;

	//wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);

	wl_surface_attach(win->wl_surface, buffer, 0, 0);
	wl_surface_commit(win->wl_surface);
}

struct xdg_surface_listener xdg_surface_listener = {
	.configure = xdg_surface_configure,
};


void global(void* userdata, struct wl_registry* registry, u32 name, const char* interface, u32 version) {
	if (strcmp(interface, wl_shm_interface.name) == 0) {
		shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	}
	else if (strcmp(interface, wl_compositor_interface.name) == 0) {
		compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 4);
	}
	else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(xdg_wm_base, &xdg_wm_base_listener, NULL);
	}
}

void global_remove(void* user, struct wl_registry* registry, u32 name) {

}

struct wl_registry_listener listener = {
	.global        = global,
	.global_remove = global_remove,
};

int main() {
	struct wl_display* display = wl_display_connect(NULL);
	struct wl_registry* registry = wl_display_get_registry(display);

	wl_registry_add_listener(registry, &listener, NULL);
	wl_display_roundtrip(display);

	window_state win = {0};
	win.wl_surface = wl_compositor_create_surface(compositor);
	win.xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, win.wl_surface);
	xdg_surface_add_listener(win.xdg_surface, &xdg_surface_listener, &win);
	win.xdg_toplevel = xdg_surface_get_toplevel(win.xdg_surface);
	xdg_toplevel_set_title(win.xdg_toplevel, "A window");
	wl_surface_commit(win.wl_surface);

	usz frame_count = 0;
	u64 start_us = time_us();
	while (wl_display_dispatch(display)) {
		for (usz i = 0; i < 800 * 600; ++i)
			win.buffer_data[i] = time_ms();

		wl_surface_damage(win.wl_surface, 0, 0, 800, 600);
		wl_surface_attach(win.wl_surface, win.buffer, 0, 0);
		wl_surface_commit(win.wl_surface);

		if (++frame_count % 100 == 0) {
			u64 us = time_us() - start_us;
			double s = (double)us / 1000000;
			double fps = (double)frame_count / s;
			lprintf("{usz} {u32}\n", frame_count, (u32)fps);
		}
	}

	return 0;
}

