#include "comicoption.h"
#include "renderer.h"
#include <stdlib.h>

void comic_option_init(comic_option_t* comicoption, char* filename, int listpos)
{
    comicoption->listpos = listpos;
    comicoption->ypos = 0;
    comicoption->visible = false;
    comicoption->selected = false;
    comicoption->name = NULL;
    comicoption->thumbnail = NULL;
    comicoption->filename = filename;
}


void comic_option_destroy(comic_option_t* comicoption)
{
    if(comicoption->name) {
        sprite_destroy(comicoption->name);
    }
    if(comicoption->thumbnail) {
        sprite_destroy(comicoption->thumbnail);
    }
    if(comicoption->filename) {
        free(comicoption->filename);
    }
}

void comic_option_make_name(comic_option_t* comicoption, text_render_t* textrender)
{
    if (comicoption->name) {
        free(comicoption->name);
    }
    comicoption->name = malloc(sizeof(sprite_t));
    sprite_init(comicoption->name);
    comicoption->name->screen = GFX_BOTTOM;
    sprite_set_texture(comicoption->name, render_text(textrender, comicoption->filename, 20, color(0,0,0)));
}

bool is_visible(comic_option_t* comicoption, int yoffset)
{
    if (yoffset > (comicoption->listpos + 1)*ITEM_SPACING) {
        return false;
    }
    if (yoffset+240 < (comicoption->listpos)*ITEM_SPACING) {
        return false;
    }
    return true;
}

bool is_selected(comic_option_t* comicoption, int touchpos)
{
    if (touchpos < 0) {
        return false;
    }
    if (comicoption->ypos > touchpos) {
        return false;
    }
    if (comicoption->ypos+ITEM_SPACING < touchpos) {
        return false;
    }

    return true;
}

void comic_option_update(comic_option_t* comicoption, int yoffset, int touchpos)
{
    comicoption->visible = is_visible(comicoption, yoffset);
    if (comicoption->visible) {
        comicoption->ypos = (comicoption->listpos)*ITEM_SPACING - yoffset;
        comicoption->name->y = comicoption->ypos + 10;
        comicoption->name->x = 30;
    }
    
    comicoption->selected = is_selected(comicoption, touchpos);

}

void comic_option_draw(comic_option_t* comicoption)
{
    if (comicoption->visible) {
        color_t c;
        if (comicoption->selected) {
            c = color(0xff,0 , 0);
        }
        else {
            c = color(0, 0 , 0);
        }
        draw_square(GFX_BOTTOM, GFX_LEFT, c,
            20, comicoption->ypos, 280, ITEM_HEIGHT, 2);

        draw_sprite(comicoption->name);
    }
}














