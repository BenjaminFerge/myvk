#include "myvk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void validation(myvk_ctx* ctx, VkInstanceCreateInfo* create)
{
    if (!ctx->debug) {
        create->enabledLayerCount = 0;
        return;
    }
    create->enabledLayerCount = ctx->layerc;
    create->ppEnabledLayerNames = ctx->layerv;
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
    validation(ctx, &create);

    uint32_t extc = 0;
    const char** exts;
    exts = glfwGetRequiredInstanceExtensions(&extc);
    create.enabledExtensionCount = extc;
    create.ppEnabledExtensionNames = exts;

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
    vkDestroyInstance(ctx->inst, NULL);
    glfwDestroyWindow(ctx->window);
    glfwTerminate();
    free(ctx);
}
