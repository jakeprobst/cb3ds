#include "texture.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void dprintf(char* str, ...)
{
    va_list valist;
    va_start(valist, str);
    char buf[1024];
    int i = vsnprintf(buf, 1024, str, valist);
    svcOutputDebugString(buf, i);
    va_end(valist);
}

void texture_init(texture_t *texture, int width, int height, int alpha)
{
    texture->data = NULL;
    texture->width = width;
    texture->height = height;
    texture->alpha = alpha;
    texture->colortype = COLOR_BGR;
    texture_alloc_data(texture);
}

void texture_init2(texture_t *texture, int width, int height, int alpha, color_type_t type)
{
    texture->data = NULL;
    texture->width = width;
    texture->height = height;
    texture->alpha = alpha;
    texture->colortype = type;
    texture_alloc_data(texture);
}

/*void texture_init(texture_t *texture, u8 *data, int w, int h)
{
    texture->data = data;
    texture->width = w;
    texture->height = h;
}*/

// TODO: mono alpha?
void texture_alloc_data(texture_t* texture)
{
    if (texture->data)
        free(texture->data);
    if (texture->alpha) {
        texture->data = malloc(texture->width*texture->height*4);
    }
    else if (texture->colortype == COLOR_MONO) {
        texture->data = malloc(texture->width*texture->height);
    }
    else {
        texture->data = malloc(texture->width*texture->height*3);
    }
}
// todo: needs to handle alpha?
texture_t* texture_scale(texture_t* texture, int w, int h)
{
    texture_t* newtex = malloc(sizeof(texture_t));
    texture_init(newtex, w, h, 0);
    newtex->colortype = texture->colortype;

    int xratio = ((texture->width << 16) / w) + 1;
    int yratio = ((texture->height << 16) / h) + 1;

    for(int x = 0; x < w; x++) {
        for(int y = 0; y < h; y++) {
            int xoff = (x*xratio) >> 16;
            int yoff = (y*yratio) >> 16;
            int dst = y * w + x;
            int src = yoff*texture->width+xoff;
            if (texture->colortype == COLOR_MONO) {
                newtex->data[dst] = texture->data[src];
            }
            else {
                newtex->data[dst*3 + 0] = texture->data[src*3 + 0];
                newtex->data[dst*3 + 1] = texture->data[src*3 + 1];
                newtex->data[dst*3 + 2] = texture->data[src*3 + 2];
            }
        }
    }

    return newtex;
}


void texture_destroy(texture_t *texture)
{
    if (texture->data) {
        free(texture->data);
    }
}




