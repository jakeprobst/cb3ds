#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <3ds/types.h>
#include <3ds.h>
#include <string.h>

void dprintf(char* str, ...);

typedef enum color_type_t {
    COLOR_RGB,
    COLOR_BGR,
    COLOR_MONO
} color_type_t;

typedef struct texture_t {
    int width;
    int height;
    int alpha;
    u8* data;
    color_type_t colortype;
} texture_t;



void texture_init(texture_t*, int, int, int);
void texture_init2(texture_t *texture, int width, int height, int alpha, color_type_t type);
void texture_alloc_data(texture_t*);
void texture_destroy(texture_t*);

texture_t* texture_scale(texture_t* texture, int w, int h);












#endif /* _TEXTURE_H_ */
