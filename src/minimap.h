#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#include "sprite.h"

typedef struct minimap_t {
    sprite_t* minimap;
    sprite_t* page;
    double xratio, yratio;
    struct {
        int x, y, w, h;
    } rect;
    struct {
        int x, y;
    } point;
    struct {
        int w, h;
    } size;
} minimap_t;


void minimap_init(minimap_t*);
void minimap_destroy(minimap_t*);

void minimap_new_page(minimap_t* minimap, sprite_t*);
void minimap_update_page(minimap_t* minimap);
void minimap_update(minimap_t*);
//void minimap_new_page(minimap_t*);















#endif /* _MINIMAP_H_ */
