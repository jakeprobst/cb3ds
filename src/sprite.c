#include "sprite.h"



void sprite_init(sprite_t *sprite)
{
    sprite->screen = GFX_TOP;
    sprite->side = GFX_LEFT;
    sprite->x = sprite->y = 0;
    sprite->subrect.x = 0;
    sprite->subrect.y = 0;
    sprite->subrect.w = 0;
    sprite->subrect.h = 0;
    sprite->texture = NULL;
}

void sprite_destroy(sprite_t *sprite)
{
}

void sprite_set_texture(sprite_t* sprite, texture_t* texture)
{
    sprite->texture = texture;
    sprite->subrect.x = 0;
    sprite->subrect.y = 0;
    sprite->subrect.w = texture->width;
    sprite->subrect.h = texture->height;
}

/*int sprite_width(sprite_t* sprite)
{
    if (sprite->subtex == 1) {
        return sprite->subrect.w;
    }
    return sprite->texture->width;
}*/
