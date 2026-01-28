#pragma once

#ifdef __unix__
#	define ON_UNIX 1
#endif

#ifdef __linux__
#	define ON_LINUX   1
#elif defined(_WIN32) || defined(WIN32)
#	define ON_WINDOWS 1
#else
#	warn unsupported platform
#endif


#ifdef __clang__
#	define IS_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
#	define IS_GCC   1
#elif defined(_MSC_VER)
#	define IS_MSVC  1
#else
#	warn unsupported compiler
#endif

#ifdef __AVX2__
#	define HAS_AVX2 1
#	define HAS_AVX  2
#elif defined(__AVX__)
#	define HAS_AVX  1
#endif

#if defined(IS_GCC) || defined(IS_CLANG)
#	define ATTRIB(attrib) __attribute__((attrib))
#	define ALIGN(n)     ATTRIB(aligned(n))
#	define INLINE       static inline ATTRIB(always_inline)
#	define NOINLINE     ATTRIB(noinline)
#	define FLATTEN	    ATTRIB(flatten)
#	define PACKED(name) ATTRIB(packed) name
#	define NORETURN     ATTRIB(noreturn)
#	define NODISCARD    ATTRIB(warn_unused_result)
#	define NONNULL      ATTRIB(nonnull)
#	define LIKELY(x)    (__builtin_expect(!!(x), 1))
#	define UNLIKELY(x)  (__builtin_expect(!!(x), 0))
#endif

#define CAT(a, b) a ## b
#define EXCAT(a, b) CAT(a, b)

#define LABEL_ADDR(label) (&&label)
#define GOTO_ADDR(addr) goto *(addr)

#include <stdarg.h>

// ----- primitives

#include <stdint.h>
#include <stddef.h>

typedef size_t usz;
typedef long long int isz;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t b8;

typedef float  f32;
typedef double f64;

// ----- strings

typedef struct ls {
	u8* ptr;
	usz size;
} ls;

#define ls(cstr) ((struct ls) { (u8*)cstr, sizeof(cstr) - 1 })
#define lls(str, size) ((struct ls) { (u8*)str, size })

#define PATH_MAX_SIZE 1024
#define PATH_BUF_SIZE PATH_MAX_SIZE

#define FILENAME_MAX_SIZE 256
#define FILENAME_BUF_SIZE FILENAME_MAX_SIZE

extern thread_local char path_buf[PATH_BUF_SIZE];

// ----- utility macros

#define CONTAINER_OF(type, member, ptr) ((type*)(ptr) - offsetof(type, member))
#define COUNT_OF(arr) (sizeof(arr) / sizeof(*arr))

#define KB(n) ((n) * 1024L)
#define MB(n) ((n) * 1048576L)
#define GB(n) ((n) * 1073741824L)

#define DEFAULT_ALIGN 16

#define AS_CSTR(x) #x

// ----- errors

typedef enum err_code {
	SUCCESS             = 0,
	ERR_ANY             = 1,

	ERR_NO_MEMORY,
	ERR_LIMIT_EXCEEDED,
	ERR_NOT_FOUND,
	ERR_CLOSED,
	ERR_UNKNOWN,
	ERR_UNSUPPORTED,
	ERR_NOT_IMPLEMENTED,
	ERR_CORRUPT,
	ERR_DENIED,
	ERR_BAD_SYNTAX,
	ERR_OVERFLOW,
	ERR_UNDERFLOW,
	ERR_INTERRUPTED,
	ERR_PATH_TOO_LONG,
	ERR_IS_DIRECTORY,
	ERR_NOT_DIRECTORY,
	ERR_IS_FILE,
	ERR_EXISTS,
	ERR_BAD_HANDLE,
	ERR_REFUSED,
	ERR_ABORTED,
	ERR_RESET,
	ERR_BAD_ADDRESS,
	ERR_BAD_ARGUMENT,
	ERR_NOT_CONNECTED,
	ERR_NOT_EMPTY,
	ERR_EMPTY,
	ERR_BROKEN_PIPE,
	ERR_TIMED_OUT,
	ERR_END_OF_STREAM,
	ERR_RUNNING,
	ERR_UNREACHABLE,
	ERR_FEATURE_DISABLED,
	ERR_PROTOCOL,

	ERR_COUNT
} err_code;

typedef struct err {
	u16 code;
	u16 flags;
	u32 detail;
	ls  message;
} err;

#define err(code) ((struct err) { code })

#define err_ignore ((struct err*)0)
#define err_fail   ((struct err*)1)
#define err_warn   ((struct err*)2)

extern ls errcode_strtab[ERR_COUNT];

#ifdef ON_LINUX
void throw_errno(err* err);
void throw_errno_val(err* err, int code);
#endif

void throw(err* err, u8 code, const char* fmt, ...);

#define propagate(err, ...) do { if ((err)->code) return __VA_ARGS__; } while (0)

// ----- io

#define R   0b001
#define W   0b010
#define X   0b100
#define RW  0b011
#define RX  0b101
#define WX  0b110
#define RWX 0b111

typedef isz(*write_fn)(void*, const void*, usz);
typedef isz(*read_fn) (void*, void*, usz);

isz lprintf(const char* fmt, ...);
isz vlprintf_fn(write_fn fn, void* usr, const char* fmt, va_list args);
isz lprintf_fn(write_fn, void* usr, const char* fmt, ...);

isz llenf(const char* fmt, ...);
isz vllenf(const char* fmt, va_list args);

// ----- logging

typedef struct logfile_header logfile_header;
typedef struct log_sink log_sink;

// ----- files

b8 convert_path(ls path, err* err);

ls fmapall(ls path, u8 mode, err* err);
void funmap(ls mapping, err* err);

typedef int file_handle;

file_handle fcreate(ls path, u8 prot, err* err);
file_handle lfopen(ls path, u8 mode, err* err);
void lfclose(file_handle file, err* err);

b8 fsetsize(file_handle fd, usz size, err* err);

usz lfwrite(file_handle file, const void* data, usz size, err* err);
usz lfread(file_handle file, void* data, usz size, err* err);

typedef struct file_stat {
	u8 type;
	u64 size;
} file_stat;

b8 lfstat(ls path, file_stat out_stat[static 1], err* err);

#define WATCH_MODIFIED 1
#define WATCH_SAVED    2
#define WATCH_DELETED  4
#define WATCH_MOVED    8
#define WATCH_EVENTS   0x0F
#define WATCH_FAILED   0x80000000

u32 fwatch_once(ls path, u32 events, err* err);

// ----- directories

typedef struct dir_handle* dir_handle;

#define FS_ANY  0
#define FS_FILE 1
#define FS_DIR  2
#define FS_LINK 3

typedef struct dir_entry {
	u8  type;
	u32 name_size;
	u8  name[FILENAME_BUF_SIZE];
} dir_entry;

dir_handle ldopen(ls path, err* err);
void ldclose(dir_handle dir, err* err);

b8 ldnext(dir_handle dir, dir_entry ent[static 1], err* err);

// ----- memory

#define VM_HUGE_PAGES 0x01

#define VM_PAGE_SIZE      KB(4)
#define VM_PAGE_SIZE_HUGE MB(2)

typedef
struct vmap {
	u8 flags;
	u8 permit;

	b8 active;
	u8 pad[5];

	usz size;
	usz guard_size;

	void* base;
	void* guard_base;
	void* guard_end;

	void** out;
	usz* out_size;
} vmap_t;

b8 vmap(vmap_t* mappings, usz count, u32 flags, err* err);
void vunmap(vmap_t* mappings, usz count, err* err);

// ----- libc

extern int memcmp(const void* ptr1, const void* ptr2, size_t size);

extern void* memset (void* data, int c, usz size);
extern void* memcpy (void* dst, const void* src, usz size);
extern void* memmove(void* dst, const void* src, usz size);
extern int   memcmp (const void* dst, const void* src, usz size);

extern void* malloc (usz size);
extern void* realloc(void* addr, usz size);
extern void  free   (void* addr);

extern usz strlen (const char* str);
extern usz strnlen(const char* str, usz max);

// ----- async

typedef struct task {
	union {
		void* reenter_at;
		size_t running;
	};
	struct task* stack_end;
	void* userdata;
} task;

