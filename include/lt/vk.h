#ifndef LT_VK_H
#define LT_VK_H 1

#include <lt/lt.h>
#include <lt/fwd.h>

#if defined(LT_UNIX)
#	define VK_USE_PLATFORM_XCB_KHR 1
#elif defined(LT_WINDOWS)
#	define VK_USE_PLATFORM_WIN32_KHR 1
#endif

#include <lt/volk.h>

b8 lt_vk_initialize_loader(void);
void lt_vk_load_instance(VkInstance inst);

b8 lt_vk_create_window_surface(lt_window_t* window, VkInstance inst, VkAllocationCallbacks* alloc, VkSurfaceKHR* out_surface);

//VkAllocationCallbacks lt_vk_allocator_from_arena(lt_arena_t* arena);

b8 lt_vk_create_generic_instance(VkAllocationCallbacks* alloc, VkInstance* inst);

b8 lt_vk_create_debug_messenger(VkInstance inst, VkAllocationCallbacks* alloc, VkDebugUtilsMessengerEXT* messenger);

#endif
