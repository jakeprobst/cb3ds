#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <3ds.h>

#include "texture.h"
#include "sprite.h"


/*typedef struct renderer_t {
    u8* backbuf_top;
    u8* backbuf_btm;
} renderer_t;



void renderer_init(renderer_t*);

void renderer_start(renderer_t*);
void renderer_finish(renderer_t*);
void renderer_draw_sprite(renderer_t*, sprite_t*);

void renderer_destroy(renderer_t*);
*/

typedef struct color_t {
    u8 r;
    u8 g;
    u8 b;
} color_t;

color_t color(u8 r, u8 g, u8 b);

void clear_screen(gfxScreen_t, gfx3dSide_t, u8);
void draw_sprite(sprite_t*);

//void draw_line(gfxScreen_t, gfx3dSide_t,
//               int x1, int y1, int x2, int y2);

void draw_square(gfxScreen_t, gfx3dSide_t, color_t,
            int x, int y, int w, int h, int thickness);







#endif /* _RENDERER_H_ */
