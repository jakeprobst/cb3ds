#include "archivezip.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <3ds.h>
#include <zlib.h>

void debug(char* str) {
    printf("%s\n", str);
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
}
typedef struct zip_central_dir_end_t {
    u16 entries;
    u32 central_dir_offset;
    u32 central_dir_size;
} zip_central_dir_end_t;

// reverse fgetc, SEEK_END 0 first
char revgetc(FILE* file)
{
    char i;
    fseek(file, -1, SEEK_CUR);
    fread(&i, 1, 1, file);
    fseek(file, -1, SEEK_CUR);
    return i;
}

int flatten_tree(void* key, void* value, void* param)
{
    archive_zip_file_t* header = value;
    archive_zip_file_t*** array = param;

    **array = header;
    (*array)++;
    return 0;
}


zip_central_dir_end_t zip_find_central_directory(FILE* zipfile)
{
    char values[] = {0x06, 0x05, 0x4B, 0x50};
    int offset = 0;

    fseek(zipfile, 0, SEEK_END);
    while(offset < 4) {
        if (revgetc(zipfile) == values[offset]) {
            offset++;
        }
        else {
            offset = 0;
        }
    }
    
    zip_central_dir_end_t dir_end;
    fseek(zipfile, 10, SEEK_CUR);
    fread(&dir_end.entries, 2, 1, zipfile);
    fread(&dir_end.central_dir_size, 4, 1, zipfile);
    fread(&dir_end.central_dir_offset, 4, 1, zipfile);

    return dir_end;
}

int archive_zip_file_cmp(void* a, void* b)
{
    char* a1 = a;
    char* b1 = b;

    return strcmp(a1, b1);
}

/*int archive_zip_file_destroy(void* key, void* value, void* param)
{
    archive_zip_file_t* fileheader = value;
    free(fileheader->name); // also key
    free(fileheader);
    return 1;
}*/

void zip_get_file_list(archive_zip_t* zip, FILE* zipfile,
                       zip_central_dir_end_t* dir_end)
{
    avltree_t files;
    avltree_init(&files, &archive_zip_file_cmp); 
    fseek(zipfile, dir_end->central_dir_offset, SEEK_SET);
    for(int i = 0; i < dir_end->entries; i++) {
        archive_zip_file_t* fileheader = malloc(sizeof(archive_zip_file_t));
        fileheader->id = i;
        fseek(zipfile, 10, SEEK_CUR);

        u16 compression_type;
        fread(&compression_type, 2, 1, zipfile); // use this probably?

        fseek(zipfile, 4, SEEK_CUR);
        u32 crc;
        fread(&crc, 4, 1, zipfile);

        fread(&fileheader->compressed_size, 4, 1, zipfile);
        fread(&fileheader->uncompressed_size, 4, 1, zipfile);

        u16 filename_len;
        fread(&filename_len, 2, 1, zipfile);
        u16 extra_len;
        fread(&extra_len, 2, 1, zipfile);
        u16 comment_len;
        fread(&comment_len, 2, 1, zipfile);

        fseek(zipfile, 8, SEEK_CUR);
        fread(&fileheader->offset, 4, 1, zipfile);
        fileheader->name = malloc(filename_len+1);
        fread(fileheader->name, filename_len, 1, zipfile);
        fseek(zipfile, extra_len+comment_len, SEEK_CUR);

        avltree_insert(&files, fileheader->name, fileheader);
    }
    zip->files = malloc(sizeof(char*)*zip->file_count);
    avltree_foreach_inorder(&files, &flatten_tree, &zip->files);
    zip->files-=zip->file_count;
    avltree_destroy(&files);
}

int archive_zip_init(archive_zip_t* zip, char* file)
{
    zip->file = fopen(file, "rb");
    zip_central_dir_end_t dir_end = zip_find_central_directory(zip->file);
    zip->file_count = dir_end.entries;
    zip_get_file_list(zip, zip->file, &dir_end);

    return 1;
}

void decompress_data(u8* dst, u32 dstlen, u8* src, u32 srclen)
{
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;
    inflateInit2(&stream, -MAX_WBITS); // without max_wbits wont decompress?
    //pretend to check error code
    stream.avail_in = srclen;
    stream.next_in = src;
    stream.avail_out = dstlen;
    stream.next_out = dst;
    inflate(&stream, Z_NO_FLUSH);

    inflateEnd(&stream);
}

//archive_handle_t archive_zip_read(archive_zip_t* zip, char* filename)
archive_handle_t archive_zip_read(archive_zip_t* zip, int page)
{
    //archive_zip_file_t* fileheader = avltree_get(zip->files, filename);
    archive_zip_file_t* fileheader = zip->files[page];
    fseek(zip->file, fileheader->offset, SEEK_SET);

    u16 len1, len2;
    u32 size1, size2;
    fseek(zip->file, 18, SEEK_CUR);
    fread(&size1, 4, 1, zip->file);
    fread(&size2, 4, 1, zip->file);
    fread(&len1, 2, 1, zip->file);
    fread(&len2, 2, 1, zip->file);
    fseek(zip->file, len1+len2, SEEK_CUR);

    u8* comp_data = malloc(fileheader->compressed_size);
    fread(comp_data, fileheader->compressed_size, 1, zip->file);

    u8* filedata = malloc(fileheader->uncompressed_size);
    decompress_data(filedata, fileheader->uncompressed_size, comp_data, fileheader->compressed_size);

    free(comp_data);
    archive_handle_t arch;
    arch.data = filedata;
    arch.size = fileheader->uncompressed_size;
    return arch;
}


void archive_zip_destroy(archive_zip_t* zip)
{
    //avltree_foreach_inorder(zip->files, &archive_zip_file_destroy, NULL);
    for(int i = 0; i < zip->file_count; i++) {
        free(zip->files[i]->name);
        free(zip->files[i]);
    }
    //avltree_destroy(zip->files);
    free(zip->files);
}









