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
#elif defined(__LTCC__)
#	define LT_LTCC
#else
#	error Unsupported compiler
#endif

// AVX
#if defined(__AVX2__)
#	define LT_AVX2 1
#	define LT_AVX 1
#elif defined(__AVX__)
#	define LT_AVX 1
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

#	define LT_ALIGN(n) ATTRIB(aligned(n))
#	define LT_INLINE inline ATTRIB(always_inline)
#	define LT_NOINLINE ATTRIB(noinline)
#	define LT_FLATTEN ATTRIB(flatten)
#	define LT_PACKED_STRUCT(name) struct ATTRIB(packed) name
#	define LT_NORETURN ATTRIB(noreturn)
#	define LT_NODISCARD ATTRIB(warn_unused_result)
#	define LT_NONNULL ATTRIB(nonnull)
#elif defined(LT_MSVC)
#	define LT_ALIGN(n) __declspec(align(n))
#	define LT_INLINE inline __forceinline
#	define LT_NOINLINE __declspec(noinline)
#	define LT_FLATTEN
#	define LT_PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#	define LT_NORETURN __declspec(noreturn)
#	define LT_NODISCARD _Check_return_
#	define LT_NONNULL
#elif defined(LT_LTCC)
#	define LT_ALIGN(n)
#	define LT_INLINE inline
#	define LT_NOINLINE
#	define LT_FLATTEN
#	define LT_PACKED_STRUCT(name) struct name
#	define LT_NORETURN
#	define LT_NODISCARD
#	define LT_NONNULL
#endif

// Better strings that double as string views
#include <lt/primitives.h>

typedef
struct lstr {
	char* str;
	usz len;
} lstr_t;

#define LSTRI(s, l) { (s), l }
#define CLSTRI(s) { (s), sizeof((s)) - 1 }
#define NLSTRI() { NULL, 0 }

#define LSTR(s, l) ((lstr_t){ (s), l })
#define CLSTR(s) ((lstr_t){ (s), sizeof((s)) - 1 })
#define NLSTR() ((lstr_t){ NULL, 0 })

// strerror wrapper
char* lt_os_err_str(void);

// Fatal errors
void LT_NORETURN lt_ferr(lstr_t str);
void LT_NORETURN lt_ferrf(char* fmt, ...);
void LT_NORETURN lt_ferrb(lstr_t str);
void LT_NORETURN lt_ferrbf(char* fmt, ...);

#define LT_NOT_IMPLEMENTED() lt_ferrf("%S() not implemented\n", LT_FUNCTION)

// Warnings
void lt_werr(lstr_t str);
void lt_werrf(char* fmt, ...);
void lt_werrb(lstr_t str);
void lt_werrbf(char* fmt, ...);

// Debug info
void lt_ierr(lstr_t str);
void lt_ierrf(char* fmt, ...);
void lt_ierrb(lstr_t str);
void lt_ierrbf(char* fmt, ...);

// Dynamic loading
void* lt_dynl_open(char* path);
void lt_dynl_close(void* handle);
void* lt_dynl_sym(void* handle, char* sym);

char* lt_dynl_err_str(void);

// LibC
extern usz strlen(const char* str);

// The IO callback is defined here to avoid having to include
// io.h in other lt/ headers.
typedef isz(*lt_io_callback_t)(void* usr, void* data, usz size);

#endif
