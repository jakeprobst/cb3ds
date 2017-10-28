#include "image.h"
#include <strings.h>

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "imagejpeg.h"

// after some testing on large images
// I just kept adding till it stopped crashing
// or loading nothing
#define STACKSIZE 1024*1024*8

image_type_t determine_image_type(u8* data, int size)
{
    if (data[0] == 0xFF && data[1] == 0xD8 && data[size-2] == 0xFF && data[size-1] == 0xD9) {
        return JPEG;
    }
    return UNKNOWNIMAGE;
}

bool is_valid_image(u8* data, int size)
{
    return determine_image_type(data, size) != UNKNOWNIMAGE;
}

texture_t* image_read(u8* data, int size)
{
    switch(determine_image_type(data, size)) {
        case PNG:
            return NULL;
            break;
        case JPEG:
            return image_read_jpeg(data, size);
            break;
        case UNKNOWNIMAGE:
        default:
            return NULL;
            break;
    }
}

void image_read_async_init(image_read_async_t* async)
{
    async->stackptr = memalign(STACKSIZE, 32);
    async->finished = true;
    async->ready = false;
    async->abort = false;
}

void image_read_async_prepare(image_read_async_t* async, u8* data, int size)
{
    //memset(async->stackptr, 0, STACKSIZE);
    async->type = determine_image_type(data, size);
    async->data = data;
    async->size = size;
    async->finished = false;
    async->ready = false;
    async->abort = false;
    switch(async->type) {
        case PNG:
            break;
        case JPEG:
            image_read_async_init_jpeg(async, data, size);
            break;
        case UNKNOWNIMAGE:
            break;
    }
}

void image_read_async_start(image_read_async_t* async)
{
    switch(async->type) {
        case PNG:
            break;
        case JPEG:
            svcCreateThread(&async->thread, &image_read_async_jpeg, (u32)async,
                    &async->stackptr[STACKSIZE/4], 0x32, 0);
            break;
        case UNKNOWNIMAGE:
            break;
    }
}

void image_read_async_destroy(image_read_async_t* async)
{
    free(async->stackptr);
}

void image_read_async_abort(image_read_async_t* async)
{
    async->abort = true;
    // this isnt allowed! hangs system
    //while(!async->finished); // wait
}


void image_read_incremental_init(image_read_incremental_t* incremental)
{
    incremental->finished = true;
    incremental->ready = false;
    incremental->texture = malloc(sizeof(texture_t));
}
void image_read_incremental_prepare(image_read_incremental_t* incremental, u8* data, int size)
{
    incremental->type = determine_image_type(data, size);
    incremental->data = data;
    incremental->size = size;
    incremental->ready = false;
    image_read_incremental_prepare_jpeg(incremental);
}
void image_read_incremental_run(image_read_incremental_t* incremental)
{
    image_read_incremental_run_jpeg(incremental);
}
void image_read_incremental_destroy(image_read_incremental_t* incremental)
{
    texture_destroy(incremental->texture);
    free(incremental->texture);
}

