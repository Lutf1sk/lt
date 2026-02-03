#include <lt2/test.h>

#include <lt2/pixbuf.h>
#include <math.h>

int main(int argc, char** argv) {
	default_log_sink->file.color = 1;

	test ("pixbuf.c") {
		pixbuf_t pb = {
			.width  = 320,
			.height = 200,
			.data   = NULL
		};
		usz size = pb.width * pb.height * sizeof(u32);
		pb.data = malloc(size);
		u32* cmp_data = malloc(size);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < 100; ++y)
			for (usz x = 0; x < 100; ++x)
				cmp_data[y * pb.width + x] = 0xFFFFFFFF;
		pb_fill_rect(&pb, -100, -100, 200, 200, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 100; y < pb.height; ++y)
			for (usz x = 220; x < pb.width; ++x)
				cmp_data[y * pb.width + x] = 0xFFFFFFFF;
		pb_fill_rect(&pb, 220,  100,  200, 200, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 10; y < pb.height - 10; ++y)
			for (usz x = 10; x < pb.width - 10; ++x)
				cmp_data[y * pb.width + x] = 0xFFFFFFFF;
		for (usz y = 11; y < pb.height - 11; ++y)
			for (usz x = 11; x < pb.width - 11; ++x)
				cmp_data[y * pb.width + x] = 0;
		pb_draw_rect(&pb, 10,  10, pb.width - 20, pb.height - 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz x = 0; x < pb.width; ++x)
			cmp_data[110 * pb.width + x] = 0xFFFFFFFF;
		pb_draw_hline(&pb, -20, 110, pb.width + 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < pb.height; ++y)
			cmp_data[y * pb.width + 110] = 0xFFFFFFFF;
		pb_draw_vline(&pb, 110, -20, pb.height + 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz x = 0; x < pb.width; ++x)
			cmp_data[112 * pb.width + x] = 0xFFFFFFFF;
		pb_draw_line(&pb, -20, 112, pb.width + 20, 112, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < pb.height; ++y)
			cmp_data[y * pb.width + 112] = 0xFFFFFFFF;
		pb_draw_line(&pb, 112, -20, 112, pb.height + 20, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		// this fails because the current circle-drawing algorithm is imprecise
		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		for (usz y = 0; y < pb.height; ++y)
			for (usz x = 0; x < pb.width; ++x)
				cmp_data[y * pb.width + x] = (ceil(sqrt(x*x + y*y)) < 75.0) * 0xFFFFFFFF;
		pb_fill_circle(&pb, 0, 0, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		pb_fill_rect(&pb, -75, -75, 75, 75, 0xFFFFFFFF);
		pb_fill_rect(&pb, pb.width, pb.height, 75, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		pb_draw_rect(&pb, -75, -75, 75, 75, 0xFFFFFFFF);
		pb_draw_rect(&pb, pb.width, pb.height, 75, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);

		memset(cmp_data, 0, size);
		memset(pb.data, 0, size);
		pb_fill_circle(&pb, -75, -75, 75, 0xFFFFFFFF);
		pb_fill_circle(&pb, pb.width + 75, pb.height + 75, 75, 0xFFFFFFFF);
		tassert(memcmp(pb.data, cmp_data, size) == 0);
	}
}

