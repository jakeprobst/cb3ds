#include <3ds.h>
#include <string.h>
#include "cb3ds.h"







int main()
{
    cb3ds_t cb3ds;
    cb3ds_init(&cb3ds);
    cb3ds_run(&cb3ds);
    cb3ds_destroy(&cb3ds);
    gfxExit();
    return 0;
}

