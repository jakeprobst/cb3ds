#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <3ds/types.h>
#include <stdio.h>
#include <jpeglib.h>
#include "texture.h"


typedef enum image_type_t {
    JPEG,
    PNG,
    UNKNOWNIMAGE
} image_type_t;


typedef struct image_read_async_t {
    image_type_t type;
    texture_t* texture;
    u8* data;
    int size;
    bool finished;
    bool ready;
    bool abort;
    union {
        struct {
            struct jpeg_error_mgr jerr;
            struct jpeg_decompress_struct jinfo;
        } jpeg;
        struct {
        } png;
    };
    Handle thread;
    u32* stackptr;
} image_read_async_t;


typedef struct image_read_incremental_t {
    image_type_t type;
    texture_t* texture;
    u8* data;
    int size;
    bool finished;
    bool ready;
    union {
        struct {
            struct jpeg_error_mgr jerr;
            struct jpeg_decompress_struct jinfo;
            int rowspan;
        } jpeg;
        struct {
        } png;
    };
} image_read_incremental_t;

void image_read_incremental_init(image_read_incremental_t* incremental);
void image_read_incremental_prepare(image_read_incremental_t* incremental, u8* data, int size);
// needs better name
void image_read_incremental_run(image_read_incremental_t* incremental);
void image_read_incremental_destroy(image_read_incremental_t* incremental);



texture_t* image_read(u8* data, int size);

bool is_valid_image(u8* data, int size);

void image_read_async_init(image_read_async_t* async);
void image_read_async_prepare(image_read_async_t* async, u8* data, int size);
void image_read_async_start(image_read_async_t* async);
void image_read_async_destroy(image_read_async_t* async);
void image_read_async_abort(image_read_async_t* async);









#endif /* _IMAGE_H_ */
