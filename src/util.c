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

bool myvk_device_extension_support(VkPhysicalDevice device,
                                   uint32_t reqc,
                                   const char** reqv)
{
    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extc, NULL);

    VkExtensionProperties extv[extc];
    vkEnumerateDeviceExtensionProperties(device, NULL, &extc, extv);

    uint32_t miss = reqc;

    for (uint32_t i = 0; i < extc; ++i) {
        VkExtensionProperties ext = extv[i];
        for (uint32_t j = 0; j < reqc; ++j) {
            const char* req = reqv[j];
            if (strcmp(req, ext.extensionName) == 0) {
                --miss;
            }
        }
    }

    return miss == 0;
}

bool myvk_device_suitable(VkPhysicalDevice device,
                          VkSurfaceKHR surface,
                          uint32_t extc,
                          const char** extv)
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

    myvk_qfamilies families = myvk_find_qfamilies(device, surface);
    myvk_swapchain_details details = myvk_qry_swapchain(device, surface);

    bool swapchain_ok = myvk_swapchain_ok(&details);
    bool has_geometry_shader = features.geometryShader && true;
    bool ext_sup = myvk_device_extension_support(device, extc, extv);
    bool fam_ok = myvk_qfamilies_complete(&families);
    bool suitable = type_ok && features.geometryShader &&
                    myvk_qfamilies_complete(&families) &&
                    myvk_device_extension_support(device, extc, extv) &&
                    myvk_swapchain_ok(&details);
    return suitable;
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

int myvk_prefer_discrete_gpu(int gpuc,
                             VkPhysicalDevice* gpuv,
                             VkSurfaceKHR surface,
                             uint32_t extc,
                             const char** extv)
{
    int idx = -1;
    for (int i = 0; i < gpuc; ++i) {
        VkPhysicalDevice gpu = gpuv[i];
        VkPhysicalDeviceProperties props;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(gpu, &props);
        vkGetPhysicalDeviceFeatures(gpu, &features);

        if (!myvk_device_suitable(gpu, surface, extc, extv)) {
            continue;
        }
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return i;
        }
        idx = i;
    }
    return idx;
}

myvk_qfamilies myvk_find_qfamilies(VkPhysicalDevice gpu, VkSurfaceKHR surface)
{
    myvk_qfamilies families;
    families.has_gfx = false;
    families.has_present = false;

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, NULL);

    VkQueueFamilyProperties props[count];
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &count, &props);

    VkBool32 wsi = false;
    uint32_t i = 0;
    bool ok = false;
    while (i < count && !ok) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            families.gfx = i;
            families.has_gfx = true;
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &wsi);
        if (wsi) {
            families.present = i;
            families.has_present = true;
        }
        ok = myvk_qfamilies_complete(&families);
        ++i;
    }
    if (!ok) {
        fprintf(stderr, "Cannot complete queue families!\n");
        exit(1);
    }

    return families;
}

bool myvk_qfamilies_complete(myvk_qfamilies* families)
{
    return families->has_gfx && families->has_present;
}

myvk_swapchain_details myvk_qry_swapchain(VkPhysicalDevice device,
                                          VkSurfaceKHR surface)
{
    myvk_swapchain_details details = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.caps);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device, surface, &details.formatc, NULL);
    if (details.formatc > 0) {
        details.formatv = malloc(details.formatc * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, surface, &details.formatc, details.formatv);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &details.modec, NULL);
    if (details.modec > 0) {
        details.modev = malloc(details.modec * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, surface, &details.modec, details.modev);
    }

    return details;
}

bool myvk_swapchain_ok(myvk_swapchain_details* details)
{
    return details->formatc > 0 && details->modec > 0;
}

VkSurfaceFormatKHR myvk_choose_surface_format(uint32_t formatc,
                                              VkSurfaceFormatKHR* formatv)
{
    if (formatc <= 0) {
        fprintf(stderr, "No available surface formats!");
        exit(1);
    }
    // Find the best if it's available
    for (uint32_t i = 0; i < formatc; ++i) {
        VkSurfaceFormatKHR f = formatv[i];
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return f;
        }
    }
    return formatv[0];
}

VkPresentModeKHR myvk_choose_present_mode(uint32_t modec,
                                          VkPresentModeKHR* modev)
{
    for (uint32_t i = 0; i < modec; ++i) {
        VkPresentModeKHR mode = modev[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }

    // VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available
    return VK_PRESENT_MODE_FIFO_KHR;
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

VkExtent2D
myvk_choose_swap_extent(VkSurfaceCapabilitiesKHR* caps, uint32_t w, uint32_t h)
{
    if (caps->currentExtent.width != UINT32_MAX) {
        return caps->currentExtent;
    } else {
        VkExtent2D actual = {w, h};

        actual.width = MAX(caps->minImageExtent.width,
                           MIN(caps->maxImageExtent.width, actual.width));

        actual.height = MAX(caps->minImageExtent.height,
                            MIN(caps->maxImageExtent.height, actual.height));

        return actual;
    }
}
