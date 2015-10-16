#include "textrender.h"

#include <stdio.h>
#include <stdlib.h>


void text_render_init(text_render_t* textrender, char* fontpath)
{
    FT_Init_FreeType(&textrender->library);
    FT_New_Face(textrender->library, fontpath, 0, &textrender->face);
}

void text_render_destroy(text_render_t* textrender)
{
    FT_Done_Face(textrender->face);
    FT_Done_FreeType(textrender->library);
}

texture_t* render_text(text_render_t* textrender, char* str, int size, color_t color)
{
    FT_Set_Pixel_Sizes(textrender->face, 0, size);
    
    int width = 0;
    for(int i = 0; i < strlen(str); i++) {
        u32 glyphindex = FT_Get_Char_Index(textrender->face, str[i]); 
        FT_Load_Glyph(textrender->face, glyphindex, FT_LOAD_DEFAULT);
        width += textrender->face->glyph->advance.x >> 6;
    }
    texture_t* tex = malloc(sizeof(texture_t));
    texture_init(tex, width, textrender->face->size->metrics.height >> 6, 1);
    
    int xoffset = 0;
    for(int i = 0; i < strlen(str); i++) {
        u32 glyphindex = FT_Get_Char_Index(textrender->face, str[i]); 
        FT_Load_Glyph(textrender->face, glyphindex, FT_LOAD_DEFAULT);
        FT_Render_Glyph(textrender->face->glyph, FT_RENDER_MODE_NORMAL);
        FT_GlyphSlot glyph = textrender->face->glyph;

        for(int gy = 0; gy < glyph->bitmap.rows; gy++) {
            for(int gx = 0; gx < glyph->bitmap.width; gx++) {
                int dst = ((gy + (tex->height-glyph->bitmap.rows)) * tex->width + gx + xoffset) * 4;
                int src = (gy * glyph->bitmap.width + gx);
                tex->data[dst + 0] = color.b; //glyph->bitmap.buffer[src];
                tex->data[dst + 1] = color.r; //glyph->bitmap.buffer[src];
                tex->data[dst + 2] = color.g; //glyph->bitmap.buffer[src];
                tex->data[dst + 3] = glyph->bitmap.buffer[src];
            }
        }
        xoffset += glyph->advance.x >> 6;
    }

    return tex;
}
