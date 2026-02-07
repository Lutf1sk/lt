#include <lt2/debug.h>
#include <lt2/str.h>
#include <lt2/log.h>

#define __USE_GNU
#include <dlfcn.h>
#include <signal.h>

static thread_local void* thread_root_frame = NULL;

// !! TODO
#define ON_AMD64 1

static
void* get_ip(ucontext_t* uc) {
#ifdef ON_X86
	return (void*)uc->uc_mcontext.gregs[14];
#elifdef ON_AMD64
	return (void*)uc->uc_mcontext.gregs[16];
#elifdef ON_IA64
	return (void*)uc->uc_mcontext.sc_ip
#else
	return NULL;
#endif
}

void handle_sigsegv(int sig, siginfo_t* si, void* usr) {
	ucontext_t* ucx = usr;

	llogf(NULL, LOG_CRIT, "segmentation fault accessing 0x{void*}", si->si_addr);

	log_ip(NULL, get_ip(ucx));
	log_stack_trace(NULL, 2);
	exit(sig);
}

void add_debug_hooks(err* err) {
	dlopen(NULL, 0);

	struct sigaction sact = {0};
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = SA_SIGINFO;
	sact.sa_sigaction = handle_sigsegv;
	if (sigaction(SIGSEGV, &sact, NULL) < 0)
		throw_errno(err);
}

void set_root_frame_impl(void* frame) {
	thread_root_frame = frame;
}

ls symname_at(void* addr) {
	Dl_info info = {0};
	if (!dladdr(addr, &info))
		return ls("");
	if (info.dli_sname)
		return stols(info.dli_sname);
	return ls("");
}

void log_ip(log_sink* logger, void* ip) {
	Dl_info info = {0};
	dladdr(ip, &info);

	if (info.dli_sname && info.dli_fname) {
		usz offset = ip - info.dli_saddr;
		llogf(NULL, LOG_INFO, "in <0x{void*}> ('{char*}'+{usz}) in '{char*}'", ip, info.dli_sname, offset, info.dli_fname);
	}
	else if (info.dli_fname) {
		llogf(NULL, LOG_INFO, "in <0x{void*}> in '{char*}'", ip, info.dli_fname);
	}
	else {
		llogf(NULL, LOG_INFO, "in <0x{void*}>", ip);
	}
}

static
void stack_trace_ip(log_sink* logger, void* ip) {
	Dl_info info = {0};
	dladdr(ip, &info);

	if (info.dli_sname && info.dli_fname) {
		usz offset = ip - info.dli_saddr;
		llogf(NULL, LOG_INFO, "called from <0x{void*}> ('{char*}'+{usz}) in '{char*}'", ip, info.dli_sname, offset, info.dli_fname);
	}
	else if (info.dli_fname) {
		llogf(NULL, LOG_INFO, "called from <0x{void*}> in '{char*}'", ip, info.dli_fname);
	}
	else {
		llogf(NULL, LOG_INFO, "called from <0x{void*}>", ip);
	}
}

#ifdef DEBUG

NOINLINE
void log_stack_trace(log_sink* logger, usz skip_frames) {
#define TRACE(n) \
	case n: \
		if (__builtin_frame_address(n) >= thread_root_frame) \
			break; \
		ip = __builtin_extract_return_addr(__builtin_return_address(n)); \
		stack_trace_ip(logger, ip);

	void* ip;
	switch (skip_frames) {
	TRACE(0)
	TRACE(1)
	TRACE(2)
	TRACE(3)
	TRACE(4)
	TRACE(5)
	TRACE(6)
	TRACE(7)
	TRACE(8)
	TRACE(9)
	TRACE(10)
	TRACE(11)
	TRACE(12)
	TRACE(13)
	TRACE(14)
	TRACE(15)
	TRACE(16)
	TRACE(17)
	TRACE(18)
	TRACE(19)
	TRACE(20)
	TRACE(21)
	TRACE(22)
	TRACE(23)
	TRACE(24)
		break;
	}

}

#endif // DEBUG

