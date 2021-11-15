//
//  mmap_util.h
//  czlog
//
//  Created by admin on 2020/10/28.
//

#ifndef _MMAP_UTIL_H_
#define _MMAP_UTIL_H_

#include <stdio.h>

#define ZLOG_MMAP_LENGTH (40*4*1024) //160k, 内存分页，每页 4k

struct MMAP_file {
    int file_desc; //file description
    void * mmap_ptr;
    size_t len;
};

/**
 * filePath 是mmap文件完整路径，eg: “/Users/admin/Desktop/1234.mmp3”
 */
bool mmap_file_open(const char * filePath, struct MMAP_file *mmapf);

bool mmap_file_close(struct MMAP_file *mmapf);

#endif /* _ZLMMAP_UTIL_H_ */
