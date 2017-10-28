#ifndef _IMAGEJPEG_H_
#define _IMAGEJPEG_H_

#include "image.h"

#define MAX_WIDTH 1200.0



texture_t* image_read_jpeg(u8* data, int size);
void image_read_async_init_jpeg(image_read_async_t* async, u8* data, int size);
void image_read_async_jpeg(void* param);


void image_read_incremental_prepare_jpeg(image_read_incremental_t* incremental);
void image_read_incremental_run_jpeg(image_read_incremental_t* incremental);








#endif /* _IMAGEJPEG_H_ */
