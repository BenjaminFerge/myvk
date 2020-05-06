#ifndef MYVK_MYVK_H
#define MYVK_MYVK_H

#include <vulkan/vulkan.h>

typedef struct myvk_ctx {
} myvk_ctx;

myvk_ctx *myvk_init();

void myvk_free(myvk_ctx *ctx);

#endif //MYVK_MYVK_H
