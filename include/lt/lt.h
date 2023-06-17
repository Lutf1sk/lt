#ifndef LT_LT_H
#define LT_LT_H 1

// Platform
#if defined(__unix__)
#	define LT_UNIX
#endif

#if defined(__linux__)
#	define LT_LINUX
#elif defined(_WIN32) || defined(WIN32)
#	define LT_WINDOWS
#else
#	error Unsupported platform
#endif

// Compiler
#if defined(__clang__)
#	define LT_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
#	define LT_GCC
#elif defined(_MSC_VER)
#	define LT_MSVC
#else
#	error Unsupported compiler
#endif

// Useful macros
#define LT_KB(n) ((usz)(n) * 1024L)
#define LT_MB(n) ((usz)(n) * 1048576L)
#define LT_GB(n) ((usz)(n) * 1073741824L)

#define LT_FILENAME CLSTR(__FILE__)
#define LT_LINENUM ((usz)__LINE__)
#define LT_FUNCTION CLSTR((char*)__func__)

// Attributes
#if defined(LT_CLANG) || defined(LT_GCC)
#	define ATTRIB(a) __attribute__((a))

#	define LT_INLINE inline ATTRIB(always_inline)
#	define LT_FLATTEN ATTRIB(flatten)
#	define LT_PACKED_STRUCT(name) struct ATTRIB(packed) name
#	define LT_NORETURN ATTRIB(noreturn)
#	define LT_NODISCARD ATTRIB(warn_unused_result)
#	define LT_NONNULL ATTRIB(nonnull)
#elif defined(LT_MSVC)
#	define LT_INLINE inline __forceinline
#	define LT_FLATTEN
#	define LT_PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#	define LT_NORETURN __declspec(noreturn)
#	define LT_NODISCARD _Check_return_
#	define LT_NONNULL
#endif

// Better strings that double as string views
#include <lt/primitives.h>

typedef
struct lstr {
	char* str;
	usz len;
} lstr_t;

#define LSTR(s, l) ((lstr_t){ (s), l })
#define CLSTR(s) ((lstr_t){ (s), sizeof((s)) - 1 })
#define NLSTR() ((lstr_t){ NULL, 0 })

// Backtrace
void lt_backtrace(void(*callback)(char*));

// strerror wrapper
char* lt_os_err_str(void);

// Fatal errors
void LT_NORETURN lt_ferr(lstr_t str);
void LT_NORETURN lt_ferrf(char* fmt, ...);
void LT_NORETURN lt_ferrb(lstr_t str);
void LT_NORETURN lt_ferrbf(char* fmt, ...);

// Warnings
void lt_werr(lstr_t str);
void lt_werrf(char* fmt, ...);
void lt_werrb(lstr_t str);
void lt_werrbf(char* fmt, ...);

// Dynamic loading
void* lt_dynl_open(char* path);
void lt_dynl_close(void* handle);
void* lt_dynl_sym(void* handle, char* sym);

char* lt_dynl_err_str(void);

// LibC
extern usz strlen(const char* str);

// Asserts
#define LT_DEBUG

// The IO callback is defined here to avoid having to include
// io.h in other lt/ headers.
typedef isz(*lt_io_callback_t)(void* usr, void* data, usz size);

typedef
struct lt_debug_caller {
	lstr_t lt_filename;
	lstr_t lt_function;
	usz lt_linenum;
} lt_debug_caller_t;

#define LT_DEBUG_CALLER ((lt_debug_caller_t[1]){ { .lt_filename = LT_FILENAME, .lt_function = LT_FUNCTION, .lt_linenum = LT_LINENUM } })

#ifdef LT_DEBUG
int lt_assert_failed(lstr_t file, usz line, lstr_t assertion);
int lt_assert_unreachable_failed(lstr_t file, usz line);
void lt_breakpoint(void);
#	define LT_ASSERT(x) ((void)( (x) ? 0 : lt_assert_failed(LT_FILENAME, LT_LINENUM, CLSTR(#x)) ))
#	define LT_ASSERT_NOT_REACHED() lt_assert_unreachable_failed(LT_FILENAME, LT_LINENUM)
#	define LT_BREAKPOINT() lt_breakpoint()

#	define LT_DEBUG_ARGS(...) (__VA_ARGS__, lt_debug_caller_t* _lt_debug_caller)
#	define LT_DEBUG_CALL(f, ...) ((f)(__VA_ARGS__, LT_DEBUG_CALLER))
#	define LT_DEBUG_FWD(f, ...) ((f)(__VA_ARGS__, _lt_debug_caller))
#	define LT_DEBUG_FERR(msg) lt_ferrf("%S:%uz: %S", _lt_debug_caller->lt_filename, _lt_debug_caller->lt_linenum, CLSTR(msg))
#	define LT_DEBUG_FERRF(fmt, ...) lt_ferrf("%S:%uz: "fmt, _lt_debug_caller->lt_filename, _lt_debug_caller->lt_linenum, __VA_ARGS__)
#	define LT_DEBUG_WERR(msg) lt_werrf("%S:%uz: %S", _lt_debug_caller->lt_filename, _lt_debug_caller->lt_linenum, CLSTR(msg))
#	define LT_DEBUG_WERRF(fmt, ...) lt_werrf("%S:%uz: "fmt, _lt_debug_caller->lt_filename, _lt_debug_caller->lt_linenum, __VA_ARGS__)
#else
#	define LT_ASSERT(x) ((void)0)
#	define LT_ASSERT_NOT_REACHED() ((void)0)
#	define LT_BREAKPOINT() ((void)0)

#	define LT_DEBUG_ARGS(...) (__VA_ARGS__)
#	define LT_DEBUG_CALL(f, ...) ((f)(__VA_ARGS__))
#	define LT_DEBUG_FWD(f, ...) ((f)(__VA_ARGS__))
#	define LT_DEBUG_FERR(msg) lt_ferrf("%S", CLSTR(msg))
#	define LT_DEBUG_FERRF(fmt, ...) lt_ferrf(fmt, __VA_ARGS__)
#	define LT_DEBUG_WERR(msg) lt_werrf("%S", CLSTR(msg))
#	define LT_DEBUG_WERRF(fmt, ...) lt_werrf(fmt, __VA_ARGS__)
#endif

#endif
