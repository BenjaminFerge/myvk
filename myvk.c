#include "myvk.h"
#include <stdlib.h>

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
    init_window(ctx);
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
    free(ctx);
}
