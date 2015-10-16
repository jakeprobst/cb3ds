#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>

#include "cb3ds.h"
#include "comicselect.h"

#include "archivezip.h"
#include "image.h"

// plans for tonight
// * look into gpu rendering
//      possibly similar renderer struct
//      6mb of vram, decently sized images can fit in there.
//      lose the ability to pass texture buffer directly due to bgr/rgb issues
// * look into removing threading and find a gettick function
//      figure I can spend 10ms a loop decoding jpeg
//      have a image_read_incremental function instead
//      of passing it off to a thread
//      also possible lookahead for images
//



/*int dumb_function(void* key, void* value, void* bleh)
{
    char* name = key;
    archive_zip_file_t* data = value;

    printf("name: %s [%d]\n", name, data->id);
    printf("data: [%d %d]", data->offset, data->uncompressed_size);
    
    return 1;
}*/
void cb3ds_init(cb3ds_t *cb3ds)
{
    //cb3ds->renderer = malloc(sizeof(renderer_t));
    //renderer_init(cb3ds->renderer);
    //cb3ds->scene = malloc(sizeof(scene_manager_t));
    //scene_manager_init(cb3ds->scene);
}

void cb3ds_run(cb3ds_t *cb3ds)
{
    gfxInitDefault();
    //consoleInit(GFX_BOTTOM, NULL);
    
    //sprite_t *sprite = malloc(sizeof(sprite_t));
    //sprite_init(sprite);

    /*sprite_t* minimap = malloc(sizeof(sprite_t));
    sprite_init(minimap);
    minimap->screen = GFX_BOTTOM;

    sprite_t* text = malloc(sizeof(sprite_t));
    sprite_init(text);*/

    //printf("%d %d %d\n", 1 << 1, 1 << 4, 1048576 >> 10);
    
    //archive_zip_t zip;
    //archive_zip_init(&zip, "/comics/jojo8.zip");

    //avltree_foreach_inorder(zip.files, &dumb_function, NULL);

    /*text_render_t textrender;
    text_render_init(&textrender, "default.ttf");
    sprite_set_texture(text, render_text(&textrender, "asdf", 36));*/

    //archive_handle_t page = archive_zip_read(&zip, 1);
    //page.size++;
    //texture_t* tex = image_read(page.data, page.size);
    //sprite_set_texture(sprite, image_read(page.data, page.size));
    //sprite_set_texture(minimap, texture_scale(sprite->texture, 265, 122));


    //sprite->subrect.w = 400;
    //sprite->subrect.h = 240;
    //printf("%X\n", (unsigned int)sprite->texture);*/


    scene_t comicselect;
    comic_select_scene_init(&comicselect);
    scene_set(&comicselect);
    //scene_push(cb3ds->scene, &comicselect);
    //comicselect.init(comicselect.data);

    while (aptMainLoop()) {
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) break;

        //comicselect.update(comicselect.data);
        //comicselect.draw(comicselect.data);
        scene_update();
        //printf("a%ld\n", svcGetSystemTick());
        //printf("b%ld\n", osGetTime());
        //scene_draw(cb3ds->scene);
            
		//u32 kheld = hidKeysHeld();
        /*if (kheld & KEY_RIGHT) {
            sprite->subrect.x += 10;
        }
        if (kheld & KEY_DOWN) {
            sprite->subrect.y += 10;
        }
        if (kheld & KEY_LEFT) {
            sprite->subrect.x -= 10;
        }
        if (kheld & KEY_UP) {
            sprite->subrect.y -= 10;
        }*/


        //cb3ds_handle_input(cb3ds);
        //renderer_start(cb3ds->renderer);
        //draw_sprite(sprite);
        //renderer_draw_sprite(cb3ds->renderer, sprite);
        //renderer_draw_sprite(cb3ds->renderer, minimap);
        //renderer_draw_sprite(cb3ds->renderer, text);

        //renderer_finish(cb3ds->renderer);

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }
    gfxExit();
}

void cb3ds_destroy(cb3ds_t *cb3ds)
{
    //renderer_destroy(cb3ds->renderer);
}

