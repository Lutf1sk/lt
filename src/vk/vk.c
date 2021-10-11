#include <lt/vk.h>
#include <lt/mem.h>
#include <lt/io.h>

#include "../window/window_def.h"

b8 lt_vk_initialize_loader(void) {
	return volkInitialize() == VK_SUCCESS;
}

void lt_vk_load_instance(VkInstance inst) {
	volkLoadInstance(inst);
}

b8 lt_vk_create_window_surface(lt_window_t* win, VkInstance inst, VkAllocationCallbacks* alloc, VkSurfaceKHR* out_surface) {
#if defined(LT_X11)

	VkXcbSurfaceCreateInfoKHR cinf;
	cinf.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	cinf.connection = lt_conn;
	cinf.window = win->window;
	cinf.flags = 0;
	cinf.pNext = NULL;

	return vkCreateXcbSurfaceKHR(inst, &cinf, alloc, out_surface) == VK_SUCCESS;

#elif defined(LT_WIN32)

	VkWin32SurfaceCreateInfoKHR cinf;
	cinf.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	cinf.hinstance = lt_hinst;
	cinf.hwnd = win->hwnd;
	cinf.flags = 0;
	cinf.pNext = NULL;

	return vkCreateWin32SurfaceKHR(inst, &cinf, alloc, out_surface) == VK_SUCCESS;

#endif
}

b8 lt_vk_create_generic_instance(VkAllocationCallbacks* alloc, VkInstance* inst) {
	VkApplicationInfo appinf;
	appinf.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appinf.pApplicationName = "LT Vulkan";
	appinf.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appinf.pEngineName = "LT Vulkan";
	appinf.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appinf.apiVersion = VK_API_VERSION_1_2;
	appinf.pNext = NULL;

	const char* layer_names[] = {
#if defined(LT_DEBUG)
		"VK_LAYER_KHRONOS_validation",
#endif
	};

	const char* extension_names[] = {
		"VK_KHR_surface",
#if defined(LT_X11)
		"VK_KHR_xcb_surface",
#elif defined(LT_WIN32)
		"VK_KHR_win32_surface",
#endif
#if defined(LT_DEBUG)
		"VK_EXT_debug_utils"
#endif
	};

	VkInstanceCreateInfo cinf;
	cinf.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	cinf.pApplicationInfo = &appinf;
	cinf.enabledLayerCount = sizeof(layer_names) / sizeof(*layer_names);
	cinf.ppEnabledLayerNames = layer_names;
	cinf.enabledExtensionCount = sizeof(extension_names) / sizeof(*extension_names);
	cinf.ppEnabledExtensionNames = extension_names;
	cinf.flags = 0;
	cinf.pNext = NULL;

	return vkCreateInstance(&cinf, alloc, inst) == VK_SUCCESS;
}

static
VkBool32 lt_vk_debug_messenger_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data)
{
	lt_printf("VLD: %s\n\n", callback_data->pMessage);
	return VK_FALSE;
}

b8 lt_vk_create_debug_messenger(VkInstance inst, VkAllocationCallbacks* alloc, VkDebugUtilsMessengerEXT* messenger) {
	VkDebugUtilsMessageSeverityFlagBitsEXT severity = 0;
	severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	//severity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

	VkDebugUtilsMessageTypeFlagsEXT types = 0;
	types |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	types |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	types |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	VkDebugUtilsMessengerCreateInfoEXT cinf;
	cinf.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	cinf.messageSeverity = severity;
	cinf.messageType = types;
	cinf.pfnUserCallback = lt_vk_debug_messenger_callback;
	cinf.pUserData = NULL;
	cinf.flags = 0;
	cinf.pNext = NULL;

	return vkCreateDebugUtilsMessengerEXT(inst, &cinf, alloc, messenger) == VK_SUCCESS;
}

