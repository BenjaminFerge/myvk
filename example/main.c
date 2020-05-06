#include "stdio.h"
#include "myvk.h"

int main(void)
{
    myvk_ctx * ctx = myvk_init();
    printf("myvk initialized\n");
    myvk_free(ctx);
    printf("myvk freed\n");
}