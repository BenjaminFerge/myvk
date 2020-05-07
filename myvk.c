#include "myvk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkResult
create_debug_messenger(VkInstance instance,
                       const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                       const VkAllocationCallbacks* pAllocator,
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
                             const VkAllocationCallbacks* pAllocator)
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

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugcb(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    printf("[%s] %s: %s\n",
           message_severity_str(messageSeverity),
           message_type_str(messageType),
           pCallbackData->pMessage);

    return VK_FALSE;
}

void setup_debug_messenger(myvk_ctx* ctx)
{
    if (!ctx->debug)
        return;
    VkDebugUtilsMessengerCreateInfoEXT create;
    create.flags = 0;
    create.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create.pfnUserCallback = debugcb;
    create.pUserData = ctx;

    if (create_debug_messenger(ctx->inst, &create, NULL, &ctx->messenger) !=
        VK_SUCCESS) {
        fprintf(stderr, "Failed to set up debug messenger!\n");
    }
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

void enable_layers(myvk_ctx* ctx, VkInstanceCreateInfo* create)
{
    if (!ctx->debug) {
        create->enabledLayerCount = 0;
        return;
    }
    uint32_t availablec = 0;
    VkLayerProperties* availablev = available_layers(&availablec);
    printf("Avalable layers (%d):\n", availablec);

    for (int j = 0; j < availablec; ++j) {
        printf("\t%d. %s\n", j + 1, availablev[j]);
    }

    for (int i = 0; i < ctx->layerc; ++i) {
        printf("Layers[%d]: %s\n", i, ctx->layerv[i]);
    }

    uint32_t nfoundc = 0;
    const char** nfoundv = not_found_layers(ctx->layerv, ctx->layerc, &nfoundc);
    for (int k = 0; k < nfoundc; ++k) {
        fprintf(stderr, "Layer not found [%d]: %s\n", k, nfoundv[k]);
    }
    if (nfoundc) {
        exit(0);
    }
    create->enabledLayerCount = ctx->layerc;
    create->ppEnabledLayerNames = ctx->layerv;
}

void add_ext(myvk_ctx* ctx, const char* name)
{
    ctx->extv = realloc(ctx->extv, (++ctx->extc) * sizeof(*ctx->extv));
    if (!ctx->extv) {
        fprintf(stderr, "Realloc failed because memory could not be allocated");
        exit(1);
    }
    ctx->extv[ctx->extc - 1] = malloc(strlen(name) * sizeof(char*));
    ctx->extv[ctx->extc - 1] = name;
}

void enable_extensions(myvk_ctx* ctx, VkInstanceCreateInfo* create)
{
    uint32_t glfw_extc = 0;
    const char** glfw_extv = glfwGetRequiredInstanceExtensions(&glfw_extc);
    ctx->extv = malloc(glfw_extc * sizeof(*glfw_extv));
    for (int i = 0; i < glfw_extc; ++i) {
        ctx->extv[i] = glfw_extv[i];
    }
    ctx->extc = glfw_extc;

    if (ctx->debug) {
        add_ext(ctx, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        for (int i = 0; i < ctx->extc; ++i) {
            printf("Extensions[%d]: %s\n", i, ctx->extv[i]);
        }
    }
    create->enabledExtensionCount = ctx->extc;
    create->ppEnabledExtensionNames = ctx->extv;
}

void create_inst(myvk_ctx* ctx)
{
    VkApplicationInfo app;
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "myvk";
    app.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app.pEngineName = "myvk";
    app.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo create;
    create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create.pApplicationInfo = &app;

    enable_layers(ctx, &create);
    enable_extensions(ctx, &create);

    VkResult result = vkCreateInstance(&create, NULL, &ctx->inst);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "Cannot create a Vulkan instance\n");
        exit(1);
    }
}

void init_window(myvk_ctx* ctx)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    ctx->window = glfwCreateWindow(800, 600, "myvk", NULL, NULL);
}

myvk_ctx* myvk_init()
{
    myvk_ctx* ctx = malloc(sizeof(myvk_ctx));
    ctx->exit = false;
    ctx->layerc = 1;
    ctx->layerv = malloc(ctx->layerc * sizeof(char*));

    const char* layer1 = "VK_LAYER_KHRONOS_validation";
    ctx->layerv[0] = malloc(strlen(layer1) * sizeof(char));
    strcpy(ctx->layerv[0], layer1);

#ifndef NDEBUG
    ctx->debug = true;
#else
    ctx->debug = false;
#endif

    init_window(ctx);
    create_inst(ctx);
    setup_debug_messenger(ctx);
    return ctx;
}

void myvk_begin(myvk_ctx* ctx)
{
    while (!glfwWindowShouldClose(ctx->window) && !ctx->exit) {
        glfwPollEvents();
    }
}

void myvk_end(myvk_ctx* ctx)
{
    ctx->exit = true;
}

void myvk_free(myvk_ctx* ctx)
{
    if (ctx->debug)
        destroy_debug_messenger(ctx->inst, ctx->messenger, NULL);
    vkDestroyInstance(ctx->inst, NULL);
    glfwDestroyWindow(ctx->window);
    glfwTerminate();
    free(ctx);
}
