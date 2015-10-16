#ifndef _TEXTRENDER_H_
#define _TEXTRENDER_H_

#include "texture.h"
#include "renderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_BITMAP_H
#include FT_GLYPH_H

typedef struct text_render_t {
    FT_Library library;
    FT_Face face;
    /*FTC_Manager manager;
    FTC_CMapCache cmapcache;
    FTC_SBitCache sbitcache;
    FTC_SBit sbit;*/
} text_render_t;

void text_render_init(text_render_t*, char*);
void text_render_destroy(text_render_t*);


texture_t* render_text(text_render_t*, char*, int, color_t color);










#endif /* _TEXTRENDER_H_ */
