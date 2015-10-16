#include "minimap.h"
#include "renderer.h"
#include <stdlib.h>

void minimap_init(minimap_t* minimap)
{
    minimap->minimap = malloc(sizeof(sprite_t));
    sprite_init(minimap->minimap);
    minimap->minimap->screen = GFX_BOTTOM;
    minimap->page = NULL;    
    minimap->rect.x = 0;
    minimap->rect.y = 0;
    minimap->rect.w = 0;
    minimap->rect.h = 0;
}

void minimap_destroy(minimap_t* minimap)
{
    sprite_destroy(minimap->minimap);
    free(minimap->minimap);
}

void minimap_make_minimap(minimap_t* minimap, texture_t* texture)
{
    int w, h;
    if (texture->width > texture->height) {
        w = 320;
        h = texture->height*(320.0/texture->width);
        minimap->minimap->x = 0;
        minimap->minimap->y = (240-h)/2;

    }
    else {
        h = 240;
        w = texture->width*(240.0/texture->height);
        minimap->minimap->x = (320-w)/2;
        minimap->minimap->y = 0;
    }
    
    sprite_set_texture(minimap->minimap, texture_scale(texture, w, h));
}

void minimap_set_page(minimap_t* minimap, sprite_t* page)
{
    if (minimap->minimap->texture) {
        texture_destroy(minimap->minimap->texture);
        free(minimap->minimap->texture);
    }
    minimap_make_minimap(minimap, page->texture);
    minimap->page = page;
}

void minimap_update(minimap_t* minimap)
{
    if (minimap->minimap == NULL || minimap->page == NULL) {
        return;
    }
    int held = hidKeysHeld();
    if (held & KEY_TOUCH) {
        touchPosition touch;
        hidTouchRead(&touch);
        
        double xratio = (1.0*minimap->page->texture->width/minimap->minimap->texture->width);
        double yratio = (1.0*minimap->page->texture->height/minimap->minimap->texture->height);

        minimap->page->subrect.x = (touch.px-minimap->minimap->x)*xratio - 400/2;
        minimap->page->subrect.y = (touch.py-minimap->minimap->y)*yratio - 240/2;

        if (minimap->page->subrect.y < 0) {
            minimap->page->subrect.y = 0;
        }
        if (minimap->page->subrect.y > minimap->page->texture->height-240) {
            minimap->page->subrect.y = minimap->page->texture->height-240;
        }

        if (minimap->page->subrect.x < 0) {
            minimap->page->subrect.x = 0;
        }
        if (minimap->page->subrect.x > minimap->page->texture->width-400) {
            minimap->page->subrect.x = minimap->page->texture->width-400;
        }
        
        minimap->rect.w = 400/xratio;
        minimap->rect.h = 240/yratio;
        minimap->rect.x = touch.px - minimap->rect.w/2 - 1;
        if (minimap->rect.x < minimap->minimap->x)
            minimap->rect.x = minimap->minimap->x;
        if (minimap->rect.x + minimap->rect.w > (minimap->minimap->x + minimap->minimap->texture->width))
            minimap->rect.x = (minimap->minimap->x + minimap->minimap->texture->width)-minimap->rect.w;
        
        minimap->rect.y = touch.py - minimap->rect.h/2 - 1;
        if (minimap->rect.y < minimap->minimap->y)
            minimap->rect.y = minimap->minimap->y;
        if (minimap->rect.y + minimap->rect.h > (minimap->minimap->y + minimap->minimap->texture->height))
            minimap->rect.y = (minimap->minimap->y + minimap->minimap->texture->height)-minimap->rect.h;
    }
    draw_sprite(minimap->minimap);
    draw_square(GFX_BOTTOM, GFX_LEFT, color(0,0,0),
            minimap->rect.x, minimap->rect.y,
            minimap->rect.w, minimap->rect.h, 1);
}








