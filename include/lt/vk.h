#ifndef LT_VK_H
#define LT_VK_H 1

#ifndef LT_VK

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

lt_err_t lt_vk_create_window_surface(lt_window_t* window, VkInstance inst, VkAllocationCallbacks* alloc, VkSurfaceKHR* out_surface);

lt_err_t lt_vk_create_generic_instance(VkAllocationCallbacks* alloc, VkInstance* inst);
lt_err_t lt_vk_create_debug_messenger(VkInstance inst, VkAllocationCallbacks* alloc, VkDebugUtilsMessengerEXT* messenger);

#endif

#endif
