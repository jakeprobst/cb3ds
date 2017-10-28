#include "renderer.h"
#include <string.h>

#include <stdio.h>

int screen_width(gfxScreen_t screen)
{
    if (screen == GFX_TOP)
        return 400;
    return 320;
}

color_t color(u8 r, u8 g, u8 b)
{
    color_t c = {r,g,b};
    return c;
}

/*void renderer_init(renderer_t *renderer)
{
	gfxInitDefault();
}



void renderer_finish(renderer_t *renderer)
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}*/

void clear_screen(gfxScreen_t screen, gfx3dSide_t side, u8 color) 
{
    u8* fb = gfxGetFramebuffer(screen, side, NULL, NULL);
    memset(fb, color, 240*screen_width(screen)*3);
}

u8 alpha_blend(u8 bg, u8 fg, u8 alpha)
{
    //printf("alpha: %d\n", (alpha * fg + (255 - alpha) * bg));
    return (alpha * fg + (255 - alpha) * bg) / 255;
}

bool in_bounds(gfxScreen_t screen, int x, int y)
{
    if (y >= 240) {
        return false;
    }
    if (y < 0) {
        return false;
    }
    if (x > screen_width(screen)) {
        return false;
    }
    if (x < 0) {
        return false;
    }

    return true;
}

//void renderer_draw_sprite(renderer_t *renderer, sprite_t *sprite)
//TODO: x/y swap: cacheperformance
void draw_sprite(sprite_t *sprite)
{
    u8* fb = gfxGetFramebuffer(sprite->screen, sprite->side, NULL, NULL);
    for(int x = 0; x < sprite->subrect.w; x++) {
        for(int y = 0; y < sprite->subrect.h; y++) {
            if (!in_bounds(sprite->screen, x + sprite->x, y + sprite->y)) {
                continue;
            }

            int src = (sprite->subrect.x + x) + (y + sprite->subrect.y)*sprite->texture->width;
            if (sprite->texture->alpha) {
                src *= 4;
            }
            else if (sprite->texture->colortype == COLOR_MONO) {
                // src *= 1;
            }
            else {
                src *= 3;
            }
            int dst = ((x + sprite->x)*240 + (240-(sprite->y + y))-1)*3;

            if (sprite->texture->alpha) {
                fb[dst + 0] = alpha_blend(fb[dst + 0],
                                          sprite->texture->data[src + 0],
                                          sprite->texture->data[src + 3]);
                fb[dst + 1] = alpha_blend(fb[dst + 1],
                                          sprite->texture->data[src + 1],
                                          sprite->texture->data[src + 3]);
                fb[dst + 2] = alpha_blend(fb[dst + 2],
                                          sprite->texture->data[src + 2],
                                          sprite->texture->data[src + 3]);
            }
            else {
                if (sprite->texture->colortype == COLOR_RGB) {
                    fb[dst + 0] = sprite->texture->data[src + 2];
                    fb[dst + 1] = sprite->texture->data[src + 1];
                    fb[dst + 2] = sprite->texture->data[src + 0];
                }
                else if (sprite->texture->colortype == COLOR_MONO) {
                    fb[dst + 0] = sprite->texture->data[src];
                    fb[dst + 1] = sprite->texture->data[src];
                    fb[dst + 2] = sprite->texture->data[src];
                }
                else {
                    fb[dst + 0] = sprite->texture->data[src + 0];
                    fb[dst + 1] = sprite->texture->data[src + 1];
                    fb[dst + 2] = sprite->texture->data[src + 2];
                }
            }
        }
    }
}


/*void draw_line(gfxScreen_t screen, gfx3dSide_t side,
               int x1, int y1, int x2, int y2)
{
    u8* fb = gfxGetFramebuffer(screen, side, NULL, NULL);
    




}*/

void draw_square(gfxScreen_t screen, gfx3dSide_t side, color_t color,
                 int x, int y, int w, int h, int thickness)
{
    u8* fb = gfxGetFramebuffer(screen, side, NULL, NULL);
    
    for(int i = x; i < w + x; i++) {
        // top line
        for(int k = y; k < y + thickness; k++) {
            if (!in_bounds(screen, i, k)) {
                continue;
            }
            int dst = (i * 240 + (240 - k)-1)*3;
            fb[dst + 0] = color.b;
            fb[dst + 1] = color.g;
            fb[dst + 2] = color.r;
        }
        // bottom line
        for(int k = y + h; k > y + h - thickness; k--) {
            if (!in_bounds(screen, i, k)) {
                continue;
            }
            int dst = (i * 240 + (240 - k)-1)*3;
            fb[dst + 0] = color.b;
            fb[dst + 1] = color.g;
            fb[dst + 2] = color.r;
        }

    }

    for(int k = y; k < y + h; k++) {
        // left line
        for(int i = x; i < x + thickness; i++) {
            if (!in_bounds(screen, i, k)) {
                continue;
            }
            int dst = (i * 240 + (240 - k)-1)*3;
            fb[dst + 0] = color.b;
            fb[dst + 1] = color.g;
            fb[dst + 2] = color.r;
        }
        // right line
        for(int i = x + w; i > x + w - thickness; i--) {
            if (!in_bounds(screen, i, k)) {
                continue;
            }
            int dst = (i * 240 + (240 - k)-1)*3;
            fb[dst + 0] = color.b;
            fb[dst + 1] = color.g;
            fb[dst + 2] = color.r;
        }
    }
}

/*void renderer_destroy(renderer_t *renderer)
{
	gfxExit();
}*/




