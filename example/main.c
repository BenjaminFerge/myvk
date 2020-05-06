#include "myvk.h"
#include "stdio.h"

int main(void)
{
    myvk_ctx* ctx = myvk_init();
    printf("myvk initialized\n");
    myvk_begin(ctx);
    myvk_free(ctx);
    printf("myvk freed\n");
}