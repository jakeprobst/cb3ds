#include "image.h"
#include <strings.h>

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

// after some testing on large images
// I just kept adding till it stopped crashing
// or loading nothing
#define STACKSIZE 1024*1024*8

#define MAX_WIDTH 1200.0

/*char* file_suffix(char* filename)
{
    int dot = 0;
    for(int i; filename[i]; i++) {
        if (filename[i] == '.') {
            dot = i;
        }
    }

    return filename+dot + 1;
}*/

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

texture_t* image_read_jpeg(u8* data, int size)
{
    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct jinfo;
    jinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&jinfo);
    jpeg_mem_src(&jinfo, data, size);
    jpeg_read_header(&jinfo, TRUE);

    // TODO: max size option, calculate stuff for scale here
    jinfo.scale_num = 2;
    jinfo.scale_denom = 3;
    
    jpeg_calc_output_dimensions(&jinfo);

    texture_t* tex = malloc(sizeof(texture_t));
    texture_init(tex, jinfo.output_width, jinfo.output_height, 0);
    

    // so for some reason jinfo.output_components sometimes returns zero?
    //int rowspan = jinfo.image_width*(jinfo.output_components);
    int rowspan = jinfo.output_width*(jinfo.out_color_space == JCS_GRAYSCALE ? 1 : 3);
    u8* buf = malloc(rowspan);
    int offset = 0;
    jpeg_start_decompress(&jinfo);
    while (jinfo.output_scanline < jinfo.output_height) {
        jpeg_read_scanlines(&jinfo, &buf, 1);
        if (jinfo.out_color_space == JCS_GRAYSCALE) {
            for(int i = 0; i < rowspan; i++) {
                tex->data[offset + 0] = buf[i];
                tex->data[offset + 1] = buf[i];
                tex->data[offset + 2] = buf[i];
                offset += 3;
            }
        }
        else {
            for(int i = 0; i < rowspan; i+=3) {
                tex->data[offset + 2] = buf[i + 0];
                tex->data[offset + 1] = buf[i + 1];
                tex->data[offset + 0] = buf[i + 2];
                offset += 3;
            }
        }
    }
    free(buf);

    jpeg_finish_decompress(&jinfo);
    jpeg_destroy_decompress(&jinfo);

    return tex;
}

// todo: drop filename, figure out format based on data
texture_t* image_read(u8* data, int size)
{
    //return image_read_jpeg(data, size);
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

// so fucking ghetto
// there has to be a better way
void figure_out_jpeg_scale(image_read_async_t* async)
{
    if (MAX_WIDTH > async->jpeg.jinfo.image_width) {
        return;
    }

    float ratio = MAX_WIDTH/async->jpeg.jinfo.image_width;
    //if (ratio > .1) {
    //}
    if (ratio > .25) {
        async->jpeg.jinfo.scale_num = 1;
        async->jpeg.jinfo.scale_denom = 4;
    }
    if (ratio > .33) {
        async->jpeg.jinfo.scale_num = 1;
        async->jpeg.jinfo.scale_denom = 3;
    }
    if (ratio > .50) {
        async->jpeg.jinfo.scale_num = 1;
        async->jpeg.jinfo.scale_denom = 2;
    }
    if (ratio > .66) {
        async->jpeg.jinfo.scale_num = 2;
        async->jpeg.jinfo.scale_denom = 3;
    }
    if (ratio > .75) {
        async->jpeg.jinfo.scale_num = 3;
        async->jpeg.jinfo.scale_denom = 4;
    }
    if (ratio > .80) {
        async->jpeg.jinfo.scale_num = 4;
        async->jpeg.jinfo.scale_denom = 5;
    }





    //async->jpeg.jinfo.scale_num = 1;
    //async->jpeg.jinfo.scale_denom = 2;

    jpeg_calc_output_dimensions(&async->jpeg.jinfo);
}

void image_read_async_init_jpeg(image_read_async_t* async, u8* data, int size)
{
    async->jpeg.jinfo.err = jpeg_std_error(&async->jpeg.jerr);
    jpeg_create_decompress(&async->jpeg.jinfo);
    jpeg_mem_src(&async->jpeg.jinfo, data, size);
    jpeg_read_header(&async->jpeg.jinfo, TRUE);

    // TODO: max size option, calculate stuff for scale here
    async->jpeg.jinfo.dct_method = JDCT_IFAST;
    async->jpeg.jinfo.do_fancy_upsampling = false;
    async->jpeg.jinfo.two_pass_quantize = false;
    async->jpeg.jinfo.dither_mode = JDITHER_ORDERED;

    //async->jpeg.jinfo.scale_num;
    //async->jpeg.jinfo.scale_denom;
    figure_out_jpeg_scale(async);
    

    async->texture = malloc(sizeof(texture_t));
    if (async->jpeg.jinfo.out_color_space == JCS_GRAYSCALE) {
        texture_init2(async->texture, async->jpeg.jinfo.output_width,
                async->jpeg.jinfo.output_height, 0, COLOR_MONO);
    }
    else {
        texture_init2(async->texture, async->jpeg.jinfo.output_width,
                async->jpeg.jinfo.output_height, 0, COLOR_RGB);
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


void image_read_async_jpeg(void* param)
{
    image_read_async_t* async = param;
    
    int rowspan = async->jpeg.jinfo.output_width*(async->jpeg.jinfo.out_color_space == JCS_GRAYSCALE ? 1 : 3);
    //u8* buf = malloc(rowspan);
    u8* buf[4];
    //u8* buf;
    //int offset = 0;
    char zzz[128];
    jpeg_start_decompress(&async->jpeg.jinfo);
    while (async->jpeg.jinfo.output_scanline < async->jpeg.jinfo.output_height && !async->abort) {
        //buf = async->texture->data + (rowspan * async->jpeg.jinfo.output_scanline);
        buf[0] = async->texture->data + (rowspan * async->jpeg.jinfo.output_scanline);
        buf[1] = async->texture->data + (rowspan * (async->jpeg.jinfo.output_scanline+1));
        buf[2] = async->texture->data + (rowspan * (async->jpeg.jinfo.output_scanline+2));
        buf[3] = async->texture->data + (rowspan * (async->jpeg.jinfo.output_scanline+3));
        jpeg_read_scanlines(&async->jpeg.jinfo, buf, 4);
        
        sprintf(zzz, "%p", abort);
        dprintf(zzz);
        if (async->abort) {
            dprintf ("aborttime");
        }
        else
            dprintf ("not aborttime");
            
        //print("looping");
        /*if (async->jpeg.jinfo.out_color_space == JCS_GRAYSCALE) {
            for(int i = 0; i < rowspan; i++) {
                async->texture->data[offset + 0] = buf[i];
                async->texture->data[offset + 1] = buf[i];
                async->texture->data[offset + 2] = buf[i];
                offset += 3;
            }
        }
        else {
            for(int i = 0; i < rowspan; i+=3) {
                async->texture->data[offset + 2] = buf[i + 0];
                async->texture->data[offset + 1] = buf[i + 1];
                async->texture->data[offset + 0] = buf[i + 2];
                offset += 3;
            }
        }*/
        async->ready = true;
    }
    //free(buf);
    if (async->abort) {
        dprintf("aborted");
        jpeg_abort_decompress(&async->jpeg.jinfo);
    }
    else {
        jpeg_finish_decompress(&async->jpeg.jinfo);
    }
    jpeg_destroy_decompress(&async->jpeg.jinfo);
    
    sprintf(zzz, "postabort: %p", abort);
    dprintf(zzz);
    free(async->data);
    async->finished = true;
    svcExitThread();
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



