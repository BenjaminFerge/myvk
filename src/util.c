#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkResult
myvk_create_debug_messenger(VkInstance instance,
                            VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                            VkAllocationCallbacks* pAllocator,
                            VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void myvk_destroy_debug_messenger(VkInstance instance,
                                  VkDebugUtilsMessengerEXT debugMessenger,
                                  VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

const char* myvk_message_type_str(VkDebugUtilsMessageTypeFlagBitsEXT type)
{
    switch (type) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        return "GENERAL";
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        return "PERFORMANCE";
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        return "VALIDATION";
    default:
        return "UNKNOWN";
    }
}

const char*
myvk_message_severity_str(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
{
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        return "ERROR";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        return "WARNING";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        return "INFO";
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        return "VERBOSE";
    default:
        return "UNKNOWN";
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL
myvk_debugcb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
             VkDebugUtilsMessageTypeFlagsEXT messageType,
             VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
             void* pUserData)
{
    printf("[%s] %s: %s\n",
           myvk_message_severity_str(messageSeverity),
           myvk_message_type_str(messageType),
           pCallbackData->pMessage);

    return VK_FALSE;
}

VkLayerProperties* myvk_available_layers(uint32_t* count)
{
    uint32_t layerc = 0;
    vkEnumerateInstanceLayerProperties(&layerc, NULL);

    VkLayerProperties* layerv = malloc(layerc * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerc, layerv);

    *count = layerc;
    return layerv;
}

const char**
myvk_not_found_layers(const char** layers, uint32_t layerc, uint32_t* count)
{
    uint32_t c = 0;
    VkLayerProperties* all = myvk_available_layers(&c);
    int nfoundc = 0;
    const char** nfoundv = malloc(1);
    for (int i = 0; i < layerc; ++i) {
        const char* name = layers[i];
        bool found = false;
        for (int j = 0; j < c; ++j) {
            VkLayerProperties p = all[j];
            if (strcmp(name, p.layerName) == 0) {
                found = true;
            }
        }
        if (!found) {
            nfoundv = realloc(nfoundv, (++nfoundc) * sizeof(*nfoundv));
            if (!nfoundv) {
                fprintf(stderr,
                        "Realloc failed because memory could not be allocated");
                exit(1);
            }
            nfoundv[nfoundc - 1] = name;
        }
    }
    *count = nfoundc;
    return nfoundv;
}

bool myvk_device_suitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props;
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceFeatures(device, &features);

    bool type_ok = false;
#ifdef MYVK_ONLY_DISCRETE_GPU
    type_ok = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
#else
    type_ok = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
              VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
#endif

    return type_ok && features.geometryShader;
}

VkPhysicalDevice* myvk_available_phyiscal_devices(VkInstance inst,
                                                  uint32_t* count)
{
    uint32_t gpuc = 0;
    vkEnumeratePhysicalDevices(inst, &gpuc, NULL);
    if (gpuc == 0) {
        fprintf(stderr, "Failed to find a GPU with Vulkan support!");
        exit(0);
    }
    VkPhysicalDevice* gpuv = malloc(gpuc * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(inst, &gpuc, gpuv);
    *count = gpuc;
    return gpuv;
}

const char* myvk_physical_device_type_str(VkPhysicalDeviceType type)
{
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "INTEGRATED";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "DISCRETE";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        return "MAX_ENUM";
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return "OTHER";
    case VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE:
        return "RANGE_SIZE";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "VIRTUAL_GPU";
    default:
        return "UNKNOWN";
    }
}

void myvk_print_physical_device(VkPhysicalDevice gpu)
{
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(gpu, &props);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(gpu, &features);
    printf("%s\n"
           "\tID:   %d\n"
           "\tType: %s\n",
           props.deviceName,
           props.deviceID,
           myvk_physical_device_type_str(props.deviceType));
}

int myvk_prefer_discrete_gpu(int gpuc, VkPhysicalDevice* gpuv)
{
    int idx = -1;
    for (int i = 0; i < gpuc; ++i) {
        VkPhysicalDevice gpu = gpuv[i];
        VkPhysicalDeviceProperties props;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(gpu, &props);
        vkGetPhysicalDeviceFeatures(gpu, &features);

        if (!myvk_device_suitable(gpu)) {
            return -1;
        }
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return i;
        }
        idx = i;
    }
    return idx;
}
