#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "texture.h"
#include <3ds/gfx.h>

typedef struct sprite_t {
    gfxScreen_t screen;
    gfx3dSide_t side;
    int x, y;
    //int subtex;
    struct {
        int x,y,w,h;
    } subrect;
    texture_t *texture;
} sprite_t;



void sprite_init(sprite_t *sprite);

void sprite_set_texture(sprite_t* sprite, texture_t* texture);

void sprite_destroy(sprite_t* sprite);
















#endif /* _SPRITE_H_ */
