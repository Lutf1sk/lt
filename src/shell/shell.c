#include <lt/shell.h>
#include <lt/io.h>

#include <stdio.h>

lt_err_t lt_shell_exec(lstr_t command, int* out_code) {
	char cstr[4096]; // !!
	cstr[lt_sprintf(cstr, "%S", command)] = 0;
	FILE* fp = popen(cstr, "r");
	if (!fp)
		return LT_ERR_UNKNOWN; // !!

	int code = pclose(fp);
	if (out_code)
		*out_code = code;

	return LT_SUCCESS;
}