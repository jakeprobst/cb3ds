#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#include "sprite.h"

typedef struct minimap_t {
    sprite_t* minimap;
    sprite_t* page;
    struct {
        int x, y, w, h;
    } rect;
} minimap_t;


void minimap_init(minimap_t*);
void minimap_destroy(minimap_t*);

void minimap_set_page(minimap_t* minimap, sprite_t* page);
void minimap_update(minimap_t*);















#endif /* _MINIMAP_H_ */
