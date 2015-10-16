#ifndef _THUMBNAILER_H_
#define _THUMBNAILER_H_

#include <3ds.h>


typedef struct comic_thumbnailer_t {
    bool running;
    Handle thread;
    u32* stack;
} comic_thumbnailer_t;

void comic_thumbnailer_init(comic_thumbnailer_t*, void*);
void comic_thumbnailer_destroy(comic_thumbnailer_t*);













#endif /* _THUMBNAILER_H_ */
