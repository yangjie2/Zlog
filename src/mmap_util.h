//
//  mmap_util.h
//  czlog
//
//  Created by admin on 2020/10/28.
//

#ifndef _MMAP_UTIL_H_
#define _MMAP_UTIL_H_

#include <stdio.h>

#define ZLOG_MMAP_LENGTH (8*16*1024) //128KB **iOS/安卓系统中，内存页大小通常是4KB，这里的大小应该是 4KB 的整数倍**

struct MMAP_file {
    void *mmap_ptr; //内存映射后返回的指针
    size_t len; //映射的文件大小
};

/**
 * filePath 是mmap文件完整路径，eg: “/Users/admin/Desktop/1234.mmp3”
 */
bool mmap_file_open(const char * filePath, struct MMAP_file *mmapf);

bool mmap_file_close(struct MMAP_file *mmapf);

#endif /* _ZLMMAP_UTIL_H_ */
