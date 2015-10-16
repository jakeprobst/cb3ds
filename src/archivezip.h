#ifndef _ARCHIVEZIP_H_
#define _ARCHIVEZIP_H_

#include <3ds/types.h>
#include <zlib.h>
#include <stdio.h>
#include "avltree.h"

typedef struct archive_handle_t {
    u8* data;
    int size;
} archive_handle_t;

typedef struct archive_zip_file_t {
    char* name;
    int id;
    int offset;
    int compressed_size;
    int uncompressed_size;
} archive_zip_file_t;

typedef struct archive_zip_t {
    int file_count;
    FILE* file;
    archive_zip_file_t** files;
    //archive_zip_file_t* files;
    //avltree_t* files;
} archive_zip_t;

int archive_zip_init(archive_zip_t*, char*);
//archive_handle_t archive_zip_read(archive_zip_t*, char*);
archive_handle_t archive_zip_read(archive_zip_t*, int);
void archive_zip_destroy(archive_zip_t*);






#endif /* _ARCHIVEZIP_H_ */
