#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

VkResult create_debug_messenger(VkInstance instance,
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

void destroy_debug_messenger(VkInstance instance,
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

const char* message_type_str(VkDebugUtilsMessageTypeFlagBitsEXT type)
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
message_severity_str(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
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
debugcb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    printf("[%s] %s: %s\n",
           message_severity_str(messageSeverity),
           message_type_str(messageType),
           pCallbackData->pMessage);

    return VK_FALSE;
}

VkLayerProperties* available_layers(uint32_t* count)
{
    uint32_t layerc = 0;
    vkEnumerateInstanceLayerProperties(&layerc, NULL);

    VkLayerProperties* layerv = malloc(layerc * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerc, layerv);

    *count = layerc;
    return layerv;
}

const char**
not_found_layers(const char** layers, uint32_t layerc, uint32_t* count)
{
    uint32_t c = 0;
    VkLayerProperties* all = available_layers(&c);
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