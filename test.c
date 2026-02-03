#include <lt2/test.h>

int main(int argc, char** argv) {
	default_log_sink->file.color = 1;

	test ("arithmetic") {
		tassert(1 == 1);
		tassert(1 == 0);
	}
}

