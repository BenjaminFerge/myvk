#include "myvk.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void myvk_setup_debug_messenger(myvk_ctx* ctx)
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
    create.pfnUserCallback = myvk_debugcb;
    create.pUserData = ctx;

    if (myvk_create_debug_messenger(
            ctx->inst, &create, NULL, &ctx->messenger) != VK_SUCCESS) {
        fprintf(stderr, "Failed to set up debug messenger!\n");
    }
}

void myvk_enable_layers(myvk_ctx* ctx, VkInstanceCreateInfo* create)
{
    if (!ctx->debug) {
        create->enabledLayerCount = 0;
        return;
    }
    uint32_t availablec = 0;
    VkLayerProperties* availablev = myvk_available_layers(&availablec);
    printf("Avalable layers (%d):\n", availablec);

    for (int j = 0; j < availablec; ++j) {
        printf("\t%d. %s\n", j + 1, availablev[j]);
    }

    for (int i = 0; i < ctx->layerc; ++i) {
        printf("Layers[%d]: %s\n", i, ctx->layerv[i]);
    }

    uint32_t nfoundc = 0;
    const char** nfoundv =
        myvk_not_found_layers(ctx->layerv, ctx->layerc, &nfoundc);
    for (int k = 0; k < nfoundc; ++k) {
        fprintf(stderr, "Layer not found [%d]: %s\n", k, nfoundv[k]);
    }
    if (nfoundc) {
        exit(0);
    }
    create->enabledLayerCount = ctx->layerc;
    create->ppEnabledLayerNames = ctx->layerv;
}

void myvk_add_ext(myvk_ctx* ctx, const char* name)
{
    ctx->extv = realloc(ctx->extv, (++ctx->extc) * sizeof(*ctx->extv));
    if (!ctx->extv) {
        fprintf(stderr, "Realloc failed because memory could not be allocated");
        exit(1);
    }
    ctx->extv[ctx->extc - 1] = malloc(strlen(name) * sizeof(char*));
    ctx->extv[ctx->extc - 1] = name;
}

void myvk_enable_extensions(myvk_ctx* ctx, VkInstanceCreateInfo* create)
{
    uint32_t glfw_extc = 0;
    const char** glfw_extv = glfwGetRequiredInstanceExtensions(&glfw_extc);
    ctx->extv = malloc(glfw_extc * sizeof(*glfw_extv));
    for (int i = 0; i < glfw_extc; ++i) {
        ctx->extv[i] = glfw_extv[i];
    }
    ctx->extc = glfw_extc;

    if (ctx->debug) {
        myvk_add_ext(ctx, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        for (int i = 0; i < ctx->extc; ++i) {
            printf("Extensions[%d]: %s\n", i, ctx->extv[i]);
        }
    }
    create->enabledExtensionCount = ctx->extc;
    create->ppEnabledExtensionNames = ctx->extv;
}

void myvk_create_inst(myvk_ctx* ctx)
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

    myvk_enable_layers(ctx, &create);
    myvk_enable_extensions(ctx, &create);

    VkResult result = vkCreateInstance(&create, NULL, &ctx->inst);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "Cannot create a Vulkan instance\n");
        exit(1);
    }
}

void myvk_init_window(myvk_ctx* ctx)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    ctx->window = glfwCreateWindow(800, 600, "myvk", NULL, NULL);
}

void myvk_init_vulkan(myvk_ctx* ctx)
{
    myvk_create_inst(ctx);
    myvk_setup_debug_messenger(ctx);
    myvk_create_surface(ctx);
    myvk_pick_physical_device(ctx);
    myvk_create_logical_device(ctx);
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

    myvk_init_window(ctx);

    ctx->physical_device = VK_NULL_HANDLE;
    myvk_init_vulkan(ctx);
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
    vkDestroyDevice(ctx->device, NULL);
    vkDestroySurfaceKHR(ctx->inst, ctx->surface, NULL);
    if (ctx->debug)
        myvk_destroy_debug_messenger(ctx->inst, ctx->messenger, NULL);
    vkDestroyInstance(ctx->inst, NULL);
    glfwDestroyWindow(ctx->window);
    glfwTerminate();
    free(ctx);
}

void myvk_pick_physical_device(myvk_ctx* ctx)
{
    uint32_t dc = 0;
    VkPhysicalDevice* dv = myvk_available_phyiscal_devices(ctx->inst, &dc);
    int idx = myvk_prefer_discrete_gpu(dc, dv, ctx->surface);
    if (idx != -1) {
        VkPhysicalDevice gpu = dv[idx];
        ctx->physical_device = gpu;
        if (ctx->debug) {
            printf("Selected GPU:\n");
            myvk_print_physical_device(gpu);
        }
    }

    if (ctx->physical_device == VK_NULL_HANDLE) {
        fprintf(stderr, "Failed to find any suitable GPU!\n");
        exit(0);
    }
}

void myvk_create_logical_device(myvk_ctx* ctx)
{
    myvk_qfamilies qfamilies =
        myvk_find_qfamilies(ctx->physical_device, ctx->surface);

    uint32_t uniqc;
    if (qfamilies.gfx == qfamilies.present)
        uniqc = 1;
    else
        uniqc = 2;

    uint32_t* uniqv = malloc(uniqc * sizeof(uint32_t));
    uniqv[0] = qfamilies.gfx;
    if (uniqc == 2) {
        uniqv[1] = qfamilies.present;
    }

    VkDeviceQueueCreateInfo* qinfos =
        malloc(uniqc * sizeof(VkDeviceQueueCreateInfo));
    float prio = 1.0f;
    for (uint32_t i = 0; i < uniqc; ++i) {
        uint32_t idx = uniqv[i];
        VkDeviceQueueCreateInfo qinfo = {};
        qinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qinfo.queueFamilyIndex = idx;
        qinfo.queueCount = 1;
        qinfo.pQueuePriorities = &prio;
        qinfos[i] = qinfo;
    }

    VkDeviceCreateInfo device_info = {};
    VkPhysicalDeviceFeatures features = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pQueueCreateInfos = qinfos;
    device_info.queueCreateInfoCount = uniqc;
    device_info.pEnabledFeatures = &features;
    device_info.enabledExtensionCount = 0;

    if (ctx->debug) {
        device_info.enabledLayerCount = ctx->layerc;
        device_info.ppEnabledLayerNames = ctx->layerv;
    } else {
        device_info.enabledLayerCount = 0;
    }

    VkDevice device;
    ctx->device = device;

    if (vkCreateDevice(
            ctx->physical_device, &device_info, NULL, &ctx->device) !=
        VK_SUCCESS) {
        fprintf(stderr, "Failed to create a logical device!\n");
        exit(1);
    }
    vkGetDeviceQueue(ctx->device, qfamilies.gfx, 0, &ctx->gfx_queue);
}

void myvk_create_surface(myvk_ctx* ctx)
{
    if (glfwCreateWindowSurface(ctx->inst, ctx->window, NULL, &ctx->surface) !=
        VK_SUCCESS) {
        fprintf(stderr, "Failed to create window surface!\n");
        exit(0);
    }
}
