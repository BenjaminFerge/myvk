#ifndef MYVK_UTIL_H
#define MYVK_UTIL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdbool.h>

VkResult
myvk_create_debug_messenger(VkInstance instance,
                            VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                            VkAllocationCallbacks* pAllocator,
                            VkDebugUtilsMessengerEXT* pDebugMessenger);

void myvk_destroy_debug_messenger(VkInstance instance,
                                  VkDebugUtilsMessengerEXT debugMessenger,
                                  VkAllocationCallbacks* pAllocator);

const char* myvk_message_type_str(VkDebugUtilsMessageTypeFlagBitsEXT type);

const char*
myvk_message_severity_str(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

const char**
myvk_not_found_layers(const char** layers, uint32_t layerc, uint32_t* count);

VkLayerProperties* myvk_available_layers(uint32_t* count);

bool myvk_device_suitable(VkPhysicalDevice device);

VkPhysicalDevice* myvk_available_phyiscal_devices(VkInstance inst,
                                                  uint32_t* count);

int myvk_prefer_discrete_gpu(int gpuc, VkPhysicalDevice* gpuv);

const char* myvk_physical_device_type_str(VkPhysicalDeviceType type);

void myvk_print_physical_device(VkPhysicalDevice gpu);

VKAPI_ATTR VkBool32 VKAPI_CALL
myvk_debugcb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
             VkDebugUtilsMessageTypeFlagsEXT messageType,
             VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
             void* pUserData);
#endif // MYVK_UTIL_H
