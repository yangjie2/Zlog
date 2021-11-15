//
//  mmap_util.c
//  czlog
//
//  Created by admin on 2020/10/28.
//

#include "mmap_util.h"
#include <string>
#include <fcntl.h>
#include "zlutils.h"
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

bool mmap_file_open(const char * filePath, struct MMAP_file *mmapf) {
    if (NULL == filePath || 0 == strnlen(filePath, 128) || NULL == mmapf) {
        return false;
    }
    unsigned char *p_map = NULL;
    int fd = open(filePath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP); //后两个添加权限
    bool isNeedCheck = false;
    if (fd == -1) {
        zl_printf("open(%s) fail: %s\n", filePath, strerror(errno));
        return false;
    }
    bool isFileOk = false;
    FILE *file = fopen(filePath, "rb+"); //先判断文件是否有值，再mmap内存映射
    if (NULL != file) {
        fseek(file, 0, SEEK_END);
        long longBytes = ftell(file);
        if (longBytes < ZLOG_MMAP_LENGTH) {
            fseek(file, 0, SEEK_SET);
            char zero_data[ZLOG_MMAP_LENGTH];
            memset(zero_data, 0, ZLOG_MMAP_LENGTH);
            size_t _size = 0;
            _size = fwrite(zero_data, sizeof(char), ZLOG_MMAP_LENGTH, file);
            fflush(file);
            if (_size == ZLOG_MMAP_LENGTH) {
               zl_printf("copy data to mmap file success\n");
                isFileOk = true;
                isNeedCheck = true;
            } else {
                isFileOk = false;
            }
        } else {
            isFileOk = true;
        }
        fclose(file);
    } else {
        isFileOk = false;
    }

    if (isNeedCheck) { //加强保护，对映射的文件要有一个适合长度的文件
        FILE *file = fopen(filePath, "rb");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            long longBytes = ftell(file);
            if (longBytes >= ZLOG_MMAP_LENGTH) {
                isFileOk = 1;
            } else {
                isFileOk = 0;
            }
            fclose(file);
        } else {
            isFileOk = 0;
        }
    }

    if (isFileOk) {
        p_map = (unsigned char *) mmap(0, ZLOG_MMAP_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }
    bool f = false;
    if (p_map != MAP_FAILED && NULL != p_map && isFileOk) {
        f = true;
    } else {
        f = false;
        zl_printf("open mmap fail , reason : %s \n", strerror(errno));

    }
    close(fd);

    if (f && access(filePath, F_OK) != -1) { //如果存在mmap文件才用mmap
        f = true;
        mmapf->file_desc = fd;
        mmapf->mmap_ptr = p_map;
        mmapf->len = ZLOG_MMAP_LENGTH;
    } else { //否则解除内存映射
        f = false;
        if (NULL != p_map) {
            munmap(p_map, ZLOG_MMAP_LENGTH);
        }
    }
    return f;
}

bool mmap_file_close(struct MMAP_file *mmapf) {
    if (NULL == mmapf) {
        return false;
    }
    int fd = mmapf->file_desc;
    char *p = (char *)mmapf->mmap_ptr;
    size_t len = mmapf->len;
    msync(p, len, MS_ASYNC);
    if (munmap(p, len) == 0 && close(fd) == 0) {
        return true;
    }
    return false;
}
