#include "minimap.h"
#include "renderer.h"
#include <stdlib.h>

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

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
    minimap->point.x = 0;
    minimap->point.y = 0;
    minimap->size.w = 0;
    minimap->size.h = 0;
}

void minimap_destroy(minimap_t* minimap)
{
    sprite_destroy(minimap->minimap);
    free(minimap->minimap);
}

void minimap_make_minimap(minimap_t* minimap, texture_t* texture)
{
    if (texture->width > texture->height) {
        minimap->size.w = 320;
        minimap->size.h = texture->height*(320.0/texture->width);
        minimap->minimap->x = 0;
        minimap->minimap->y = (240-minimap->size.h)/2;

    }
    else {
        minimap->size.h = 240;
        minimap->size.w = texture->width*(240.0/texture->height);
        minimap->minimap->x = (320-minimap->size.w)/2;
        minimap->minimap->y = 0;
    }
    
    sprite_set_texture(minimap->minimap, texture_scale(texture, minimap->size.w, minimap->size.h));
}

void minimap_move_viewport(minimap_t* minimap, int x, int y)
{
    minimap->rect.w = 400/minimap->xratio;
    minimap->rect.h = 240/minimap->yratio;
    minimap->rect.x = x - minimap->rect.w/2 - 1;
    if (minimap->rect.x < minimap->minimap->x)
        minimap->rect.x = minimap->minimap->x;
    if (minimap->rect.x + minimap->rect.w > (minimap->minimap->x + minimap->minimap->texture->width))
        minimap->rect.x = (minimap->minimap->x + minimap->minimap->texture->width)-minimap->rect.w;
    
    minimap->rect.y = y - minimap->rect.h/2 - 1;
    if (minimap->rect.y < minimap->minimap->y)
        minimap->rect.y = minimap->minimap->y;
    if (minimap->rect.y + minimap->rect.h > (minimap->minimap->y + minimap->minimap->texture->height))
        minimap->rect.y = (minimap->minimap->y + minimap->minimap->texture->height)-minimap->rect.h;

}

void minimap_new_page(minimap_t* minimap, sprite_t* page)
{
    minimap->page = page;
    minimap_make_minimap(minimap, minimap->page->texture);
    minimap->xratio = (1.0*page->texture->width/minimap->minimap->texture->width);
    minimap->yratio = (1.0*page->texture->height/minimap->minimap->texture->height);
    dprintf("ratios: (%f %f) ", minimap->xratio, minimap->yratio);
    minimap->rect.w = 400/minimap->xratio;
    minimap->rect.h = 240/minimap->yratio;
    //minimap->rect.w = 0;
    //minimap->rect.h = 0;
    minimap->rect.x = 0;
    minimap->rect.y = 0;
    minimap->point.x = minimap->minimap->x + minimap->rect.w/2;
    minimap->point.y = minimap->minimap->y + minimap->rect.h/2;
    //minimap->xratio = (1.0*minimap->page->texture->width/minimap->minimap->texture->width);
    //minimap->yratio = (1.0*minimap->page->texture->height/minimap->minimap->texture->height);
    minimap_move_viewport(minimap, 0, 0);
    minimap_move_subrect(minimap, 0, 0);
}

void minimap_update_page(minimap_t* minimap)
{
    if (minimap->minimap->texture) {
        texture_destroy(minimap->minimap->texture);
        free(minimap->minimap->texture);
    }
    //sprite_set_texture(minimap->minimap, texture_scale(texture, w, h));
    sprite_set_texture(minimap->minimap, texture_scale(minimap->page->texture, minimap->size.w, minimap->size.h));
}

void minimap_move_subrect(minimap_t* minimap, int x, int y)
{
    minimap->page->subrect.x = (x-minimap->minimap->x)*minimap->xratio - 400/2;
    minimap->page->subrect.y = (y-minimap->minimap->y)*minimap->yratio - 240/2;

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

        minimap->point.x = touch.px;
        minimap->point.y = touch.py;

        minimap_move_subrect(minimap, touch.px, touch.py);
        minimap_move_viewport(minimap, touch.px, touch.py);
    }
    if (held & KEY_UP) {
        minimap->point.y = MAX(minimap->point.y - 3, minimap->minimap->y + minimap->rect.h/2);
        minimap_move_viewport(minimap, minimap->point.x, minimap->point.y);
        minimap_move_subrect(minimap, minimap->point.x, minimap->point.y);
    }
    if (held & KEY_DOWN) {
        // TODO: figure why this is sliiiiightly too big still
        minimap->point.y = MIN(minimap->point.y + 3, minimap->minimap->y + minimap->size.h);
        minimap_move_viewport(minimap, minimap->point.x, minimap->point.y);
        minimap_move_subrect(minimap, minimap->point.x, minimap->point.y);
    }
    if (held & KEY_LEFT) {
        minimap->point.x = MAX(minimap->point.x - 3, minimap->minimap->x + minimap->rect.w/2);
        minimap_move_viewport(minimap, minimap->point.x, minimap->point.y);
        minimap_move_subrect(minimap, minimap->point.x, minimap->point.y);
    }
    if (held & KEY_RIGHT) {
        //minimap->point.x += 3;
        minimap->point.x = MIN(minimap->point.x + 3, minimap->minimap->x + minimap->size.w);
        minimap_move_viewport(minimap, minimap->point.x, minimap->point.y);
        minimap_move_subrect(minimap, minimap->point.x, minimap->point.y);
    }

    /*dprintf("[%d %d %d %d] (%d %d)",
            minimap->rect.x, minimap->rect.y,
            minimap->rect.w, minimap->rect.h,
            minimap->point.x, minimap->point.y
            );*/
    
    
    draw_sprite(minimap->minimap);
    draw_square(GFX_BOTTOM, GFX_LEFT, color(0,0,0),
            minimap->rect.x, minimap->rect.y,
            minimap->rect.w, minimap->rect.h, 1);
}








