#ifndef MYVK_MYVK_H
#define MYVK_MYVK_H

#define GLFW_INCLUDE_VULKAN
#include "util.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct myvk_ctx {
    GLFWwindow* window;
    bool exit;
    bool debug;
    VkInstance inst;
    VkDebugUtilsMessengerEXT messenger;
    const char** layerv;
    int layerc;
    uint32_t extc;
    const char** extv;
    uint32_t device_extc;
    const char** device_extv;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue gfx_queue;
    VkSurfaceKHR surface;
    VkQueue present_queue;
} myvk_ctx;

myvk_ctx* myvk_init();

void myvk_free(myvk_ctx* ctx);

void myvk_begin(myvk_ctx* ctx);

void myvk_end(myvk_ctx* ctx);

void myvk_setup_debug_messenger(myvk_ctx* ctx);

void myvk_enable_layers(myvk_ctx* ctx, VkInstanceCreateInfo* create);

void myvk_add_ext(myvk_ctx* ctx, const char* name);

void myvk_enable_extensions(myvk_ctx* ctx, VkInstanceCreateInfo* create);

void myvk_create_inst(myvk_ctx* ctx);

void myvk_init_window(myvk_ctx* ctx);

void myvk_init_vulkan(myvk_ctx* ctx);

void myvk_pick_physical_device(myvk_ctx* ctx);

void myvk_create_logical_device(myvk_ctx* ctx);

void myvk_create_surface(myvk_ctx* ctx);

#endif // MYVK_MYVK_H
