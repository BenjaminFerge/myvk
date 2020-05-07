#ifndef MYVK_MYVK_H
#define MYVK_MYVK_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdbool.h>

typedef struct myvk_ctx {
    GLFWwindow* window;
    bool exit;
    VkInstance inst;
} myvk_ctx;

myvk_ctx* myvk_init();

void myvk_free(myvk_ctx* ctx);
void myvk_begin(myvk_ctx* ctx);
void myvk_end(myvk_ctx* ctx);

#endif // MYVK_MYVK_H
