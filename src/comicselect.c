#include "comicselect.h"
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

#include "renderer.h"
#include "sprite.h"
#include "textrender.h"
#include "comicoption.h"
#include "thumbnailer.h"
#include "comicview.h"

typedef struct comic_select_scene_t {
    int comic_count;
    comic_option_t** comic_options;
    text_render_t textrender;
    int yoffset;
    //sprite_t* test;
    //comic_thumbnailer_t thumbnailer;
} comic_select_scene_t;


void comic_select_init(void* data)
{
    comic_select_scene_t* scene = data;
    scene->comic_count = 0;
    
    DIR* files = opendir("/comics/");
    struct dirent* entry;
    while((entry = readdir(files))) {
        scene->comic_count++;
    }
    scene->comic_options = malloc(sizeof(comic_option_t*)*scene->comic_count);

    // rewinddir isnt implemented so just close and reopen
    closedir(files);
    files = opendir("/comics/");
    for(int cindex = 0; (entry = readdir(files)); cindex++) {
        comic_option_t* comicoption = malloc(sizeof(comic_option_t));
        comic_option_init(comicoption, strdup(entry->d_name), cindex);
        //comic_option_init(comicoption, "placeholder", cindex);
        
        scene->comic_options[cindex] = comicoption;
    }
    closedir(files);

    text_render_init(&scene->textrender, "default.ttf");

    for(int i = 0; i < scene->comic_count; i++) {
        comic_option_t* opt = scene->comic_options[i];
        comic_option_make_name(opt, &scene->textrender);
    }
    
    /*scene->test = malloc(sizeof(sprite_t));
    sprite_init(scene->test);
    texture_t* texture = malloc(sizeof(texture_t));
    texture_init(texture, 400, 240, 0);
    sprite_set_texture(scene->test, texture);
    for(int x = 0; x < texture->width; x++) {
        for(int y = 0; y < texture->height; y++) {
            texture->data[(y * texture->width + x)*3 + 0] = 0x00;
            texture->data[(y * texture->width + x)*3 + 1] = 0x00;
            texture->data[(y * texture->width + x)*3 + 2] = 0x00;
        }
        }*/
}

void comic_select_draw(void* data)
{
    clear_screen(GFX_TOP, GFX_LEFT, 0xdd);
    clear_screen(GFX_BOTTOM, GFX_LEFT, 0xdd);

    comic_select_scene_t* scene = data;
    for(int i = 0; i < scene->comic_count; i++) {
        comic_option_draw(scene->comic_options[i]);
    }
}

comic_option_t* get_selected_comic(comic_option_t** opts, int count)
{
    for(int i = 0; i < count; i++) {
        if (opts[i]->selected) {
            return opts[i];
        }
    }
    return NULL;
}

void comic_select_update(void* data)
{
    static int prevtouch = -1;
    comic_select_scene_t* scene = data;
	u32 kHeld = hidKeysHeld();
	u32 kDown = hidKeysDown();
	if (kHeld & KEY_UP) {
        scene->yoffset -= 6;
    }
	if (kHeld & KEY_DOWN) {
        scene->yoffset += 6;
    }

    //if (kHeld & KEY_TOUCH) {
        touchPosition touch;
        hidTouchRead(&touch);
        if (prevtouch == -1) {
            prevtouch = touch.py;
        }
        scene->yoffset -= touch.py - prevtouch;
        prevtouch = touch.py;
        //}
//else {
//      prevtouch = -1;
//  }
    
    if (kDown & KEY_A) {
        scene_t view;
        comic_option_t* opt = get_selected_comic(scene->comic_options, scene->comic_count);
        if (opt) {
            comic_view_scene_init(&view, opt->filename);
            scene_set(&view);
            return;
        }
    }


    /*if (scene->yoffset > 0) {
        scene->yoffset = 0;
    }
    if (scene->yoffset > scene->comic_count*ITEM_SPACING) {
        scene->yoffset = scene->comic_count*ITEM_SPACING;
    }*/
    for(int i = 0; i < scene->comic_count; i++) {
        comic_option_update(scene->comic_options[i], scene->yoffset, prevtouch);
    }

    comic_select_draw(data);
}



void comic_select_destroy(void* data)
{
    comic_select_scene_t* scene = data;
    text_render_destroy(&scene->textrender);
    for(int i = 0;  i < scene->comic_count; i++) {
        comic_option_destroy(scene->comic_options[i]);
        free(scene->comic_options[i]);
    }
    free(scene->comic_options);
}

void athing()
{
    printf("hahahaha\n");
}

void comic_select_scene_init(scene_t* scene)
{
    scene->data = malloc(sizeof(comic_select_scene_t));
    scene->init = &comic_select_init;
    scene->destroy = &comic_select_destroy;
    scene->update = &comic_select_update;
}
