#ifndef MYVK_UTIL_H
#define MYVK_UTIL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdbool.h>

typedef struct myvk_qfamilies {
    uint32_t gfx;
    bool has_gfx;
    uint32_t present;
    bool has_present;
} myvk_qfamilies;

typedef struct myvk_swapchain_details {
    VkSurfaceCapabilitiesKHR caps;
    VkSurfaceFormatKHR* formatv;
    uint32_t formatc;
    VkPresentModeKHR* modev;
    uint32_t modec;
} myvk_swapchain_details;

myvk_swapchain_details myvk_qry_swapchain(VkPhysicalDevice device,
                                          VkSurfaceKHR surface);

bool myvk_swapchain_ok(myvk_swapchain_details* details);

bool myvk_qfamilies_complete(myvk_qfamilies* families);

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

bool myvk_device_extension_support(VkPhysicalDevice device,
                                   uint32_t reqc,
                                   const char** reqv);

bool myvk_device_suitable(VkPhysicalDevice device,
                          VkSurfaceKHR surface,
                          uint32_t extc,
                          const char** extv);

VkPhysicalDevice* myvk_available_phyiscal_devices(VkInstance inst,
                                                  uint32_t* count);

int myvk_prefer_discrete_gpu(int gpuc,
                             VkPhysicalDevice* gpuv,
                             VkSurfaceKHR surface,
                             uint32_t extc,
                             const char** extv);

const char* myvk_physical_device_type_str(VkPhysicalDeviceType type);

void myvk_print_physical_device(VkPhysicalDevice gpu);

myvk_qfamilies myvk_find_qfamilies(VkPhysicalDevice gpu, VkSurfaceKHR surface);

VKAPI_ATTR VkBool32 VKAPI_CALL
myvk_debugcb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
             VkDebugUtilsMessageTypeFlagsEXT messageType,
             VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
             void* pUserData);
#endif // MYVK_UTIL_H
