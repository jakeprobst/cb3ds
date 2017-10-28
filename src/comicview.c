#include "comicview.h"
#include "archive.h"
#include "sprite.h"
#include "renderer.h"
#include "image.h"
#include "textrender.h"
#include "minimap.h"

#include <string.h>
#include <stdlib.h>


typedef struct comic_view_scene_t {
    char* filename;
    int current_page;
    bool wantnextpage;
    sprite_t* pageimage;
    minimap_t minimap;

    archive_zip_t zip;

    //image_read_async_t imgasync;
    image_read_incremental_t imgincr;
} comic_view_scene_t;

void comic_view_next_page(comic_view_scene_t* cview)
{
    cview->current_page++;
    if (cview->current_page > cview->zip.file_count) {
        return;
    }
    
    archive_handle_t page = archive_zip_read(&cview->zip, cview->current_page);
    if (cview->pageimage->texture) {
        texture_destroy(cview->pageimage->texture);
    }
 
    if (is_valid_image(page.data, page.size)) {
        image_read_incremental_prepare(&cview->imgincr, page.data, page.size);
        image_read_incremental_run(&cview->imgincr);
        sprite_set_texture(cview->pageimage, cview->imgincr.texture);
    }
    else {
        free(page.data);
        comic_view_next_page(cview);
    }
}

void comic_view_init(void* data)
{
    comic_view_scene_t* cview = data;
    cview->current_page = -1;
    cview->wantnextpage = false;
    minimap_init(&cview->minimap);

    char buf[1024];
    snprintf(buf, 1024, "/comics/%s", cview->filename);
    archive_zip_init(&cview->zip, buf);

    cview->pageimage = malloc(sizeof(sprite_t));
    sprite_init(cview->pageimage);

    //text_render_t text;
    //text_render_init(&text, "default.ttf");
    //sprite_set_texture(cview->pageimage, render_text(&text, cview->zip.files[1]->name, 20, color(0,0,0)));

    //image_read_async_init(&cview->imgasync);
    
    //comic_view_next_page(cview);
    //comic_view_check_next_page_avail(cview);

    //archive_handle_t page = archive_zip_read(&cview->zip, 2);
    
    //sprite_set_texture(cview->pageimage, image_read(page.data, page.size));
    //image_read_async(cview->pageimage->texture, page.data, page.size);
    //image_read_async_t async;
    //image_read_async_init(&cview->imgasync, page.data, page.size);
    //sprite_set_texture(cview->pageimage, cview->imgasync.texture);
    //image_read_async_start(&cview->imgasync);
    
    //minimap_set_page(&cview->minimap, cview->pageimage);
    image_read_incremental_init(&cview->imgincr);
    comic_view_next_page(cview);
    minimap_new_page(&cview->minimap, cview->pageimage);

    //cview->pageimage->subrect.x = 0;
    //cview->pageimage->subrect.y = 0;
    cview->pageimage->subrect.w = 400;
    cview->pageimage->subrect.h = 240;
}

void comic_view_destroy(void* data)
{
    comic_view_scene_t* cview = data;
    //image_read_async_destroy(&cview->imgasync);
    image_read_incremental_destroy(&cview->imgincr);
}



void comic_view_update(void* data)
{
    comic_view_scene_t* cview = data;
    clear_screen(GFX_BOTTOM, GFX_LEFT, 0xff);

    int kdown = hidKeysDown();
    if (kdown & KEY_A) {
        comic_view_next_page(cview);
        minimap_new_page(&cview->minimap, cview->pageimage);
        //dprintf("look at this! page: %p", &cview->pageimage);
        //cview->minimap.finished = false;
        //image_read_async_abort(&cview->imgasync);
        //cview->imgasync.abort = true;
    }

    //comic_view_check_next_page_avail(cview);
    //int time = svcGetSystemTicks();
    bool still_reading = false;
    u64 time = osGetTime();
    while (!cview->imgincr.finished && time + 30 > osGetTime()) {
        image_read_incremental_run(&cview->imgincr);
        //minimap_new_page(&cview->minimap, cview->pageimage);
        //dprintf("new page: %p", &cview->pageimage);
        //dprintf("new page: %p", &cview->pageimage);
        still_reading = true;
    }

    if (cview->imgincr.ready) {
        draw_sprite(cview->pageimage);
        if (still_reading) {
            minimap_update_page(&cview->minimap);
        }
    }
    minimap_update(&cview->minimap);
}





void comic_view_scene_init(scene_t* scene, char* filename)
{
    comic_view_scene_t* cview = malloc(sizeof(comic_view_scene_t));
    cview->filename = strdup(filename);
    scene->data = cview;

    scene->init = &comic_view_init;
    scene->destroy = &comic_view_destroy;
    scene->update = &comic_view_update;
}
