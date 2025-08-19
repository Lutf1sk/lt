#include <lt2/common.h>

#define err_store ((struct err*)3)

void fail(const err err[static 1]) {
	lprintf("error: {ls}\n", err->message); // !! should write to stderr
}

void warn(const err err[static 1]) {
	lprintf("warning: {ls}\n", err->message); // !! should write to stderr
}

void throw(err* err, u8 code, const char* fmt, ...) {
	if (err == err_ignore)
		return;

	struct err errval = err(code);

	if (err >= err_store) {
		*err = errval;
		return;
	}

	errval.message = lls((u8*)fmt, strlen(fmt));
	if (err == err_fail)
		fail(&errval);
	else if (err == err_warn)
		warn(&errval);
}

