#ifndef LT_VK_H
#define LT_VK_H 1

#ifdef LT_VK

#include <lt/err.h>
#include <lt/fwd.h>

#if defined(LT_UNIX)
#	define VK_USE_PLATFORM_XCB_KHR 1
#elif defined(LT_WINDOWS)
#	define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <lt/volk.h>

lt_err_t lt_vk_initialize_loader(void);
void lt_vk_load_instance(VkInstance inst);
void lt_vk_load_device(VkDevice device);

lt_err_t lt_vk_create_window_surface(lt_window_t* window, VkInstance inst, VkAllocationCallbacks* alloc, VkSurfaceKHR* out_surface);

lt_err_t lt_vk_create_generic_instance(
		VkAllocationCallbacks* alloc,
		usz layer_count, const char* layer_names[static layer_count],
		usz ext_count, const char* ext_names[static ext_count],
		VkInstance* inst);

#endif

#endif
