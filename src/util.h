#ifndef MYVK_UTIL_H
#define MYVK_UTIL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VkResult create_debug_messenger(VkInstance instance,
                                VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                VkAllocationCallbacks* pAllocator,
                                VkDebugUtilsMessengerEXT* pDebugMessenger);

void destroy_debug_messenger(VkInstance instance,
                             VkDebugUtilsMessengerEXT debugMessenger,
                             VkAllocationCallbacks* pAllocator);

const char* message_type_str(VkDebugUtilsMessageTypeFlagBitsEXT type);

const char*
message_severity_str(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

const char**
not_found_layers(const char** layers, uint32_t layerc, uint32_t* count);

VkLayerProperties* available_layers(uint32_t* count);

VKAPI_ATTR VkBool32 VKAPI_CALL
debugcb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
#endif // MYVK_UTIL_H
