#include <lt2/test.h>

#include <lt2/pixbuf.h>


int main(int argc, char** argv) {
	default_log_sink->file.color = 1;

	test ("pixbuf") {
		pixbuf_t pb = {
			.width  = 320,
			.height = 200,
			.data   = malloc(320 * 200 * sizeof(u32))
		};

		pb_fill_rect(&pb, -100, -100, 200, 200, 0xFFFFFFFF);
		pb_fill_rect(&pb, 220,  100,  200, 200, 0xFFFFFFFF);
		tassert(1 == 1);
	}
}

