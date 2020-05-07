#include "myvk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void enable_layers(myvk_ctx* ctx, VkInstanceCreateInfo* create)
{
    if (!ctx->debug) {
        create->enabledLayerCount = 0;
        return;
    }
    for (int i = 0; i < ctx->layerc; ++i) {
        printf("Layers[%d]: %s\n", i, ctx->layerv[i]);
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
