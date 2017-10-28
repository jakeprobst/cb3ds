#include "imagejpeg.h"
#include <stdlib.h>

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


// so fucking ghetto
// there has to be a better way
void figure_out_jpeg_scale(struct jpeg_decompress_struct* jinfo)
{
    /*if (MAX_WIDTH > jinfo->image_width) {
        return;
    }*/

    float ratio = MAX_WIDTH/jinfo->image_width;
    //if (ratio > .1) {
    //}
    if (ratio > .25) {
        jinfo->scale_num = 1;
        jinfo->scale_denom = 4;
    }
    if (ratio > .33) {
        jinfo->scale_num = 1;
        jinfo->scale_denom = 3;
    }
    if (ratio > .50) {
        jinfo->scale_num = 1;
        jinfo->scale_denom = 2;
    }
    if (ratio > .66) {
        jinfo->scale_num = 2;
        jinfo->scale_denom = 3;
    }
    if (ratio > .75) {
        jinfo->scale_num = 3;
        jinfo->scale_denom = 4;
    }
    if (ratio > .80) {
        jinfo->scale_num = 4;
        jinfo->scale_denom = 5;
    }

    jinfo->scale_num = 1;
    jinfo->scale_denom = 1;

    jpeg_calc_output_dimensions(jinfo);
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

    figure_out_jpeg_scale(&async->jpeg.jinfo);

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

void image_read_async_jpeg(void* param)
{
    image_read_async_t* async = param;
    
    int rowspan = async->jpeg.jinfo.output_width*(async->jpeg.jinfo.out_color_space == JCS_GRAYSCALE ? 1 : 3);
    u8* buf[4];
    jpeg_start_decompress(&async->jpeg.jinfo);
    while (async->jpeg.jinfo.output_scanline < async->jpeg.jinfo.output_height && !async->abort) {
        //buf = async->texture->data + (rowspan * async->jpeg.jinfo.output_scanline);
        buf[0] = async->texture->data + (rowspan * async->jpeg.jinfo.output_scanline);
        buf[1] = async->texture->data + (rowspan * (async->jpeg.jinfo.output_scanline+1));
        buf[2] = async->texture->data + (rowspan * (async->jpeg.jinfo.output_scanline+2));
        buf[3] = async->texture->data + (rowspan * (async->jpeg.jinfo.output_scanline+3));
        jpeg_read_scanlines(&async->jpeg.jinfo, buf, 4);
        
        async->ready = true;
    }
    //free(buf);
    if (async->abort) {
        jpeg_abort_decompress(&async->jpeg.jinfo);
    }
    else {
        jpeg_finish_decompress(&async->jpeg.jinfo);
    }
    jpeg_destroy_decompress(&async->jpeg.jinfo);
    
    free(async->data);
    async->finished = true;
    svcExitThread();
}

void image_read_incremental_prepare_jpeg(image_read_incremental_t* incr)
{
    if (!incr->finished) {
        jpeg_abort_decompress(&incr->jpeg.jinfo);
        //jpeg_finish_decompress(&incr->jpeg.jinfo);
        jpeg_destroy_decompress(&incr->jpeg.jinfo);
        free(incr->data);
    }

    incr->jpeg.jinfo.err = jpeg_std_error(&incr->jpeg.jerr);
    jpeg_create_decompress(&incr->jpeg.jinfo);
    jpeg_mem_src(&incr->jpeg.jinfo, incr->data, incr->size);
    jpeg_read_header(&incr->jpeg.jinfo, TRUE);

    // TODO: max size option, calculate stuff for scale here
    incr->jpeg.jinfo.dct_method = JDCT_IFAST;
    incr->jpeg.jinfo.do_fancy_upsampling = false;
    incr->jpeg.jinfo.two_pass_quantize = false;
    incr->jpeg.jinfo.dither_mode = JDITHER_ORDERED;

    figure_out_jpeg_scale(&incr->jpeg.jinfo);
    /*incr->jpeg.jinfo.scale_num = 2;
    incr->jpeg.jinfo.scale_denom = 3;
    jpeg_calc_output_dimensions(&incr->jpeg.jinfo);*/
    dprintf("%d %d -> %d %d", incr->jpeg.jinfo.image_width, incr->jpeg.jinfo.image_height,
            incr->jpeg.jinfo.output_width, incr->jpeg.jinfo.output_height);

    //incr->texture = malloc(sizeof(texture_t));
    dprintf("tex: %p", incr->texture);
    if (incr->jpeg.jinfo.out_color_space == JCS_GRAYSCALE) {
        texture_init2(incr->texture, incr->jpeg.jinfo.output_width,
                incr->jpeg.jinfo.output_height, 0, COLOR_MONO);
    }
    else {
        texture_init2(incr->texture, incr->jpeg.jinfo.output_width,
                incr->jpeg.jinfo.output_height, 0, COLOR_RGB);
    }
    incr->jpeg.rowspan = incr->jpeg.jinfo.output_width*(incr->jpeg.jinfo.out_color_space == JCS_GRAYSCALE ? 1 : 3);
    //incr->jpeg.rowspan = incr->jpeg.jinfo.output_width*incr->jpeg.jinfo.out_color_components;
    jpeg_start_decompress(&incr->jpeg.jinfo);
    incr->finished = false;
}

void image_read_incremental_run_jpeg(image_read_incremental_t* incr)
{
    if (incr->finished) {
        return;
    }
    //int rowspan = incr->jpeg.jinfo.output_width*(incr->jpeg.jinfo.out_color_space == JCS_GRAYSCALE ? 1 : 3);
    u8* buf[4];
    buf[0] = incr->texture->data + (incr->jpeg.rowspan * incr->jpeg.jinfo.output_scanline);
    buf[1] = incr->texture->data + (incr->jpeg.rowspan * (incr->jpeg.jinfo.output_scanline+1));
    buf[2] = incr->texture->data + (incr->jpeg.rowspan * (incr->jpeg.jinfo.output_scanline+2));
    buf[3] = incr->texture->data + (incr->jpeg.rowspan * (incr->jpeg.jinfo.output_scanline+3));
    jpeg_read_scanlines(&incr->jpeg.jinfo, buf, 4);
    
    incr->ready = true;
    if (incr->jpeg.jinfo.output_scanline == incr->jpeg.jinfo.output_height) {
        jpeg_finish_decompress(&incr->jpeg.jinfo);
        jpeg_destroy_decompress(&incr->jpeg.jinfo);
        free(incr->data);
        incr->finished = true;
    }
}

