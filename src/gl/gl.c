#include <lt/gl.h>

b8 lt_gl_initialize_loader(void) {
	return !!gladLoadGL();
}

