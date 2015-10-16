#include "thumbnailer.h"
#include <malloc.h>

#define STACKSIZE (1024 * 4)

void thumbnailer_func(void* data)
{
    svcExitThread();
}





void comic_thumbnailer_init(comic_thumbnailer_t* thumbnailer, void* data)
{
    thumbnailer->running = true;
    thumbnailer->stack = memalign(32, STACKSIZE);
    svcCreateThread(&thumbnailer->thread,
                    thumbnailer_func,
                    (u32)data,
                    &thumbnailer->stack[STACKSIZE/4],
                    0x3f,
                    0);
}

void comic_thumbnailer_destroy(comic_thumbnailer_t* thumbnailer)
{

}












