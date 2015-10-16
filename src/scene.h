#ifndef _SCENE_H_
#define _SCENE_H_

struct scene_manager_t;

typedef struct scene_t {
    void* data;
    void (*init)(void*);
    void (*destroy)(void*);
    void (*update)(void*);
    //void (*draw)(void*);
} scene_t;



void scene_manager_init();
void scene_set(scene_t*);
//void scene_replace(scene_manager_t*, scene_t*);
//void scene_pop(scene_manager_t*);

void scene_update();
//void scene_draw(scene_manager_t*);









#endif /* _SCENE_H_ */
