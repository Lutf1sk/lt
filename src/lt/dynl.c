#include <lt/lt.h>

#if defined(LT_UNIX)
#	include <dlfcn.h>
#	define DYNL_OPEN(path) dlopen((path), (RTLD_LAZY))
#	define DYNL_CLOSE(hnd) dlclose((hnd))
#	define DYNL_SYM(hnd, name) dlsym((hnd), (name))
#	define DYNL_ERR_STR() dlerror()
#elif defined(LT_WINDOWS)
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#	define DYNL_OPEN(path) LoadLibrary((path))
#	define DYNL_CLOSE(hnd) FreeLibrary((hnd))
#	define DYNL_SYM(hnd, name) GetProcAddress((hnd), (name))
#	define DYNL_ERR_STR() strerror(GetLastError())
#endif

void* lt_dynl_open(char* path) {
	return DYNL_OPEN(path);
}

void lt_dynl_close(void* handle) {
	DYNL_CLOSE(handle);
}

void* lt_dynl_sym(void* handle, char* sym) {
	return DYNL_SYM(handle, sym);
}

char* lt_dynl_err_str(void) {
	char* err = DYNL_ERR_STR();
	return err ? err : "No error";
}

