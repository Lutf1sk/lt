#include <lt2/cli.h>
#include <lt2/str.h>

#ifdef ON_UNIX
#	include <unistd.h>
#elifdef ON_WASI
#	include <lt2/wasi.h>
#endif

static
isz write_out(void* usr, const void* data, usz size) {
	return write((i32)(usz)usr, data, size);
}

isz lprintf(const char* fmt, ...) {
	va_list arg_list;
	va_start(arg_list, fmt);
	usz res = vlprintf_fn(write_out, (void*)(usz)STDOUT_FILENO, fmt, arg_list);
	va_end(arg_list);
	return res;
}

b8 parse_cli_args(int argc, char** argv, cli_options cli[static 1], err* err) {
	if (!cli->callback) {
		throw(err, ERR_BAD_ARGUMENT, "cli argument callback is null");
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

#ifndef ON_WASI
#	include <stdio.h>


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
		self = lls(cmdline_buf, strnlen((char*)cmdline_buf, cmdline_size));

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


cli_process_t cli_run(ls cmd, err* error) {
	int out_fds[2] = {0};
	int in_fds [2] = {0};
	int err_fds[2] = {0};

	if UNLIKELY (pipe(out_fds) < 0) {
		throw_errno(error);
		goto err0;
	}
	if UNLIKELY (pipe(in_fds) < 0) {
		throw_errno(error);
		goto err1;
	}
	if UNLIKELY (pipe(err_fds) < 0) {
		throw_errno(error);
		goto err2;
	}

	pid_t child_pid = fork();
	if (!child_pid) {
		close(out_fds[0]);
		close(in_fds [1]);
		close(err_fds[0]);

		if UNLIKELY (dup2(out_fds[1], STDOUT_FILENO) < 0)
			exit(1);
		if UNLIKELY (dup2(in_fds [0], STDIN_FILENO) < 0)
			exit(1);
		if UNLIKELY (dup2(err_fds[1], STDERR_FILENO) < 0)
			exit(1);

		close(out_fds[1]);
		close(in_fds [0]);
		close(err_fds[1]);

		char* cstr = malloc(cmd.size + 1);
		memcpy(cstr, cmd.ptr, cmd.size);
		cstr[cmd.size] = 0;

		execl("/bin/sh", "/bin/sh", "-c", cstr, NULL);
		exit(1); // execl should never return if successful
	}

	close(out_fds[1]);
	close(in_fds [0]);
	close(err_fds[1]);

	return (cli_process_t) {
		.out = out_fds[0],
		.in  = in_fds [1],
		.err = err_fds[0],
		.pid = child_pid,
	};

err2:
	close(in_fds[0]);
	close(in_fds[1]);
err1:
	close(out_fds[0]);
	close(out_fds[1]);
err0:
	return (cli_process_t) {
		.out = -1,
		.in  = -1,
		.err = -1,
	};
}

void cli_close(cli_process_t* p, err* error) {
	// !! TODO: error handling
	close(p->out);
	close(p->in);
	close(p->err);
}


#endif // !ON_WASI

