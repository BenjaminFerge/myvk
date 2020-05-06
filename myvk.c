#include "myvk.h"
#include <stdio.h>
#include <stdlib.h>

myvk_ctx* myvk_init()
{
    return malloc(sizeof(myvk_ctx));
}

void myvk_free(myvk_ctx* ctx)
{
    free(ctx);
}
