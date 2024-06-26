#ifndef LT_LT_H
#define LT_LT_H 1

// Platform
#if defined(__unix__)
#	define LT_UNIX 1
#endif

#if defined(__linux__)
#	define LT_LINUX 1
#elif defined(_WIN32) || defined(WIN32)
#	define LT_WINDOWS 1
#else
#	error Unsupported platform
#endif

// Compiler
#if defined(__clang__)
#	define LT_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
#	define LT_GCC 1
#elif defined(_MSC_VER)
#	define LT_MSVC 1
#elif defined(__LTCC__)
#	define LT_LTCC 1
#else
#	error Unsupported compiler
#endif

// Architecture
#if defined(__i386__)
#	define LT_X86 1
#elif defined(__X86_64__) || defined(__x86_64__)
#	define LT_AMD64 1
#elif defined(__ia64__)
#	define LT_IA64 1
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

#define LT_CONTAINER_OF(type, member, ptr) ((type*)((u8*)(ptr) - offsetof((type), (member))))
#define LT_ARRAY_COUNT(array) (sizeof(array) / sizeof(*array))

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
#	define LT_LIKELY(x) (__builtin_expect(!!(x), 1))
#	define LT_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#elif defined(LT_MSVC)
#	define LT_ALIGN(n) __declspec(align(n))
#	define LT_INLINE inline __forceinline
#	define LT_NOINLINE __declspec(noinline)
#	define LT_FLATTEN
#	define LT_PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#	define LT_NORETURN __declspec(noreturn)
#	define LT_NODISCARD _Check_return_
#	define LT_NONNULL
#	define LT_LIKELY(x) (x)
#	define LT_UNLIKELY(x) (x)
#elif defined(LT_LTCC)
#	define LT_ALIGN(n)
#	define LT_INLINE inline
#	define LT_NOINLINE
#	define LT_FLATTEN
#	define LT_PACKED_STRUCT(name) struct name
#	define LT_NORETURN
#	define LT_NODISCARD
#	define LT_NONNULL
#	define LT_LIKELY(x) (x)
#	define LT_UNLIKELY(x) (x)
#endif

#include <lt/primitives.h>

// Better strings that double as string views

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
const char* lt_os_err_str(void);
// Fatal errors
void LT_NORETURN lt_ferrf(const char* fmt, ...);
void LT_NORETURN lt_ferrbf(const char* fmt, ...);
#define LT_NOT_IMPLEMENTED() lt_ferrf("%S() not implemented\n", LT_FUNCTION)
// Warnings
void lt_werrf(const char* fmt, ...);
void lt_werrbf(const char* fmt, ...);
// Debug info
void lt_ierrf(const char* fmt, ...);
void lt_ierrbf(const char* fmt, ...);

// Dynamic loading
void* lt_dynl_open(const char* path);
void lt_dynl_close(void* handle);
void* lt_dynl_sym(void* handle, const char* sym);
const char* lt_dynl_err_str(void);

// LibC
extern usz strlen(const char* str);

// stream callbacks
typedef isz(*lt_write_fn_t)(void* usr, const void* data, usz size);
typedef isz(*lt_read_fn_t)(void* usr, void* data, usz size);

static LT_INLINE
isz lt_writes(lt_write_fn_t callb, void* usr, const char* str) {
	return callb(usr, str, strlen(str));
}

static LT_INLINE
isz lt_writels(lt_write_fn_t callb, void* usr, lstr_t str) {
	return callb(usr, str.str, str.len);
}

// allocator interface is defined here to avoid having to include the full mem.h in other headers
typedef void*(*lt_alloc_for_caller_fn_t)(void* alloc, usz size, const void* caller);
typedef void*(*lt_alloc_fn_t)(void* alloc, usz size);
typedef void(*lt_free_fn_t)(void* alloc, const void* mem);
typedef void*(*lt_realloc_for_caller_fn_t)(void* alloc, void* mem, usz size, const void* caller);
typedef void*(*lt_realloc_fn_t)(void* alloc, void* mem, usz size);

typedef
struct lt_alloc {
	lt_alloc_fn_t alloc;
	lt_alloc_for_caller_fn_t alloc_for_caller;
	lt_free_fn_t free;
	lt_realloc_fn_t realloc;
	lt_realloc_for_caller_fn_t realloc_for_caller;
} lt_alloc_t;

#endif
