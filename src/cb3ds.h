#ifndef _CB3DS_H_
#define _CB3DS_H_

#include "renderer.h"
#include "scene.h"

typedef struct cb3ds_t {
    //renderer_t *renderer;
    //scene_manager_t* scene;
} cb3ds_t;


void cb3ds_init(cb3ds_t*);

void cb3ds_run(cb3ds_t*);
void cb3ds_handle_input(cb3ds_t*);

void cb3ds_destroy(cb3ds_t*);











#endif /* _CB3DS_H_ */





