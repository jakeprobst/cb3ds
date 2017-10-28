#include "scene.h"
#include <stdlib.h>
#include <stdio.h>

#include "texture.h"
typedef struct scene_manager_t {
    scene_t scene;
} scene_manager_t;

scene_manager_t* scene_manager;


void scene_manager_init()
{
    scene_manager = malloc(sizeof(scene_manager_t));
    printf("sm: %p", scene_manager);
    scene_manager->scene.data = NULL;
}

void scene_set(scene_t* scene)
{
    if (scene_manager->scene.data != NULL) {
        scene_manager->scene.destroy(scene_manager->scene.data);
        free(scene_manager->scene.data);
    }
    scene->init(scene->data);
    scene_manager->scene = *scene;
}

void scene_update()
{
    scene_manager->scene.update(scene_manager->scene.data);
    //dprintf("su");
}

/*void scene_manager_init(scene_manager_t* manager)
{
    manager->scenes = 0;
}

void scene_push(scene_manager_t* manager, scene_t* scene)
{
    scene->init(scene->data, manager);
    manager->scene[manager->scenes++] = *scene;
}

void scene_replace(scene_manager_t* manager, scene_t* scene)
{
    scene_t oldscene = manager->scene[manager->scenes - 1];
    oldscene.destroy(oldscene.data);
    free(oldscene.data);
    scene->init(scene->data, manager);
    manager->scene[manager->scenes - 1] = *scene;
}

void scene_pop(scene_manager_t* manager)
{
    scene_t oldscene = manager->scene[manager->scenes--];
    oldscene.destroy(oldscene.data);
    manager->scenes--;
}

void scene_update(scene_manager_t* manager)
{
    scene_t scene = manager->scene[manager->scenes - 1];
    scene.update(scene.data);
}

void scene_draw(scene_manager_t* manager)
{
    scene_t scene = manager->scene[manager->scenes - 1];
    scene.draw(scene.data);
}*/










