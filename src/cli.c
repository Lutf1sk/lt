#include <lt2/cli.h>
#include <lt2/str.h>

#include <unistd.h>

static
isz write_out(void* usr, const void* data, usz size) {
	return write((int)(isz)usr, data, size);
}

isz lprintf(const char* fmt, ...) {
	va_list arg_list;
	va_start(arg_list, fmt);
	usz res = vlprintf(write_out, (void*)STDOUT_FILENO, fmt, arg_list);
	va_end(arg_list);
	return res;
}

b8 parse_cli_args(int argc, char** argv, cli_options cli[static 1], err* err) {
	if (!cli->callback) {
		throw(err, ERR_ANY, "cli argument callback is null");
		return 0;
	}

	int i = 1;
	while (i < argc) {
		ls arg = stols(argv[i++]);

		if (lsprefix(arg, ls("--"))) {
			if (arg.size == 2)
				break;

			usz eq = lsfirst(arg, '=');
			ls key = lsdrop(arg, 2);
			if (eq != arg.size)
				key = lstake(key, eq - 2);

			for (usz j = 0; j < cli->param_count; ++j) {
				cli_param* param = &cli->params[j];
				if (!lseq(key, param->long_key))
					continue;

				if (!param->arg_type) // should error if '=' is present
					cli->callback(cli, j, ls(""));
				else if (eq != arg.size)
					cli->callback(cli, j, lsdrop(arg, eq + 1));
				else if (i < argc)
					cli->callback(cli, j, stols(argv[i++]));
				else {
					throw(err, ERR_BAD_ARGUMENT, "expected an argument to option '--{ls}'", key);
					return 0;
				}

				goto next_arg;
			}
			throw(err, ERR_BAD_ARGUMENT, "unknown option '--{ls}'", key);
			return 0;
		}

		// this does not treat '-n -l' the same as '-nl'
		if (arg.size >= 2 && arg.ptr[0] == '-') {
			char c = arg.ptr[1];
			for (usz j = 0; j < cli->param_count; ++j) {
				cli_param* param = &cli->params[j];
				if (param->short_key != c)
					continue;

				if (!param->arg_type)
					cli->callback(cli, j, ls(""));
				else if (arg.size > 2)
					cli->callback(cli, j, lsdrop(arg, 2));
				else if (i < argc)
					cli->callback(cli, j, stols(argv[i++]));
				else {
					throw(err, ERR_BAD_ARGUMENT, "expected an argument to option '-{char}'", c);
					return 0;
				}

				goto next_arg;
			}
			throw(err, ERR_BAD_ARGUMENT, "unknown option '-{char}'", c);
			return 0;
		}

		cli->callback(cli, -1, arg);
	next_arg:
	}

	while (i < argc)
		cli->callback(cli, -1, stols(argv[i++]));
	return 1;
}

// TODO: this is messy. should be cleaned up.
void print_cli_help(cli_options cli[static 1]) {
	u8 cmdline_buf[2048];
	file_handle f = lfopen(ls("/proc/self/cmdline"), R, err_warn);
	usz cmdline_size = lfread(f, cmdline_buf, sizeof(cmdline_buf), err_warn);
	lfclose(f, err_warn);
	ls self;
	if (!cmdline_size)
		self = ls("<?>");
	else
		self = lls(cmdline_buf, strnlen(cmdline_buf, cmdline_size));

	if (!cli->param_count)
		lprintf("usage: {ls} ARGS...\n", self);
	else
		lprintf("usage: {ls} [OPTIONS] ARGS...\noptions:\n", self);

	usz pad_to = 0;
	for (cli_param* it = cli->params, *end = it + cli->param_count; it < end; ++it) {
		usz len = 0;
		if (it->short_key && it->long_key.size)
			len += 6 + it->long_key.size + (!!it->arg_type * 4) + 1;
		else if (it->short_key)
			len += 2 + (!!it->arg_type * 4) + 1;
		else
			len += 2 + it->long_key.size + (!!it->arg_type * 4) + 1;
		if (len > pad_to)
			pad_to = len;
	}

	if (pad_to > sizeof(cmdline_buf))
		pad_to = sizeof(cmdline_buf);
	memset(cmdline_buf, ' ', pad_to);

	for (cli_param* it = cli->params, *end = it + cli->param_count; it < end; ++it) {
		if (!it->short_key && !it->long_key.size) {
			lprintf("{ls}", it->description);
			continue;
		}

		lprintf("  ");
		if (it->short_key && it->long_key.size) {
			usz len = 6 + it->long_key.size + (!!it->arg_type * 4);

			lprintf("-{char}, --{ls}", it->short_key, it->long_key);
			if (it->arg_type)
				lprintf("=VAL");
			lprintf("{ls}{ls}\n", lls(cmdline_buf, pad_to - len), it->description);
		}
		else if (it->short_key) {
			usz len = 2 + (!!it->arg_type * 4);

			lprintf("-{char}", it->short_key);
			if (it->arg_type)
				lprintf(" VAL");
			lprintf("{ls}{ls}\n", lls(cmdline_buf, pad_to - len), it->description);
		}
		else {
			usz len = 2 + it->long_key.size + (!!it->arg_type * 4);
			if (len > pad_to)
				len = pad_to;

			lprintf("--{ls}", it->long_key);
			if (it->arg_type)
				lprintf("=VAL");
			lprintf("{ls}{ls}\n", lls(cmdline_buf, pad_to - len), it->description);
		}
	}
}

