#ifndef MYVK_MYVK_H
#define MYVK_MYVK_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct myvk_qfamilies {
    uint32_t gfx;
    bool has_gfx;
} myvk_qfamilies;

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
    VkPhysicalDevice physical_device;
    myvk_qfamilies queues;
    VkDevice device;
    VkQueue gfx_queue;
    VkSurfaceKHR surface;
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
