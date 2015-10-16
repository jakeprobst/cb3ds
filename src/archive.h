#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "archivezip.h"
#include "archiverar.h"


typedef enum archive_type_t {
    ARCHIVE_ZIP,
    ARCHIVE_RAR
} archive_type_t;

typedef struct archive_t {
    archive_type_t type;
    union {
        archive_zip_t zip;
        archive_rar_t rar;
    } archive;
} archive_t;

int archive_init(archive_t*, char*);



void archive_destroy(archive_t*);




#endif /* _ARCHIVE_H_ */
