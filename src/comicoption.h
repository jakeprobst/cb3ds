#ifndef _COMICOPTION_H_
#define _COMICOPTION_H_

#include "sprite.h"
#include "textrender.h"

#define ITEM_HEIGHT 60
#define ITEM_SPACING 64

typedef struct comic_option_t {
    int listpos;
    int ypos;
    bool visible;
    bool selected;
    sprite_t* thumbnail;
    sprite_t* name;
    char* filename;
} comic_option_t;


void comic_option_init(comic_option_t*, char*, int);
void comic_option_destroy(comic_option_t*);
void comic_option_make_name(comic_option_t*, text_render_t*);

void comic_option_update(comic_option_t*, int, int);
void comic_option_draw(comic_option_t*);


#endif /* _COMICOPTION_H_ */

