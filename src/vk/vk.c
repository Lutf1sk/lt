#ifdef LT_VK

#include <lt/lt.h>
#include <lt/vk.h>

#include "volk.c"

#include <lt/mem.h>
#include <lt/io.h>

#include "../window/window_def.h"

lt_err_t lt_vk_initialize_loader(void) {
	if (volkInitialize() != VK_SUCCESS)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;
}

void lt_vk_load_instance(VkInstance inst) {
	volkLoadInstance(inst);
}

void lt_vk_load_device(VkDevice device) {
	volkLoadDevice(device);
}

lt_err_t lt_vk_create_window_surface(const lt_window_t* win, VkInstance inst, VkAllocationCallbacks* alloc, VkSurfaceKHR out_surface[static 1]) {
#if defined(LT_X11)

	VkXcbSurfaceCreateInfoKHR cinf;
	cinf.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	cinf.connection = lt_conn;
	cinf.window = win->window;
	cinf.flags = 0;
	cinf.pNext = NULL;

	if (vkCreateXcbSurfaceKHR(inst, &cinf, alloc, out_surface) != VK_SUCCESS)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;

#elif defined(LT_WIN32)

	VkWin32SurfaceCreateInfoKHR cinf;
	cinf.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	cinf.hinstance = lt_hinst;
	cinf.hwnd = win->hwnd;
	cinf.flags = 0;
	cinf.pNext = NULL;
	if (vkCreateWin32SurfaceKHR(inst, &cinf, alloc, out_surface) != VK_SUCCESS)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;

#endif
}

lt_err_t lt_vk_create_generic_instance(
		VkAllocationCallbacks* alloc,
		usz layer_count, const char* layer_names[static layer_count],
		usz ext_count, const char* ext_names[static ext_count],
		VkInstance inst[static 1])
{
	VkApplicationInfo appinf;
	appinf.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinf.pApplicationName = "LT Vulkan";
	appinf.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appinf.pEngineName = "LT Vulkan";
	appinf.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appinf.apiVersion = VK_API_VERSION_1_2;
	appinf.pNext = NULL;

	char* surface_ext_names[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
	#if defined LT_X11
		VK_KHR_XCB_SURFACE_EXTENSION_NAME,
	#elif defined(LT_WIN32)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	#endif
	};
	usz surface_ext_count = sizeof(surface_ext_names) / sizeof(*surface_ext_names);

	#define EXT_MAX 32
	const char* full_ext_names[EXT_MAX];
	usz full_ext_count = ext_count + surface_ext_count;

	LT_ASSERT(full_ext_count < EXT_MAX);
	memcpy(full_ext_names, surface_ext_names, surface_ext_count * sizeof(char*));
	memcpy(full_ext_names + surface_ext_count, ext_names, ext_count * sizeof(char*));

	VkInstanceCreateInfo cinf;
	cinf.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	cinf.pApplicationInfo = &appinf;
	cinf.enabledLayerCount = layer_count;
	cinf.ppEnabledLayerNames = layer_names;
	cinf.enabledExtensionCount = full_ext_count;
	cinf.ppEnabledExtensionNames = full_ext_names;
	cinf.flags = 0;
	cinf.pNext = NULL;

	if (vkCreateInstance(&cinf, alloc, inst) != VK_SUCCESS)
		return LT_ERR_UNKNOWN; // !!
	return LT_SUCCESS;
}

#endif