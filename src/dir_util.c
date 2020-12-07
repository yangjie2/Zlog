//
//  dir_util.c
//  czlog
//
//  Created by admin on 2020/10/28.
//

#include "dir_util.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "zlutils.h"

#define ZLOG_MAX_PATH_LENGTH 1024

int zl_makedir(const char *path) {
    size_t pathLen = strlen(path);
    char currentPath[ZLOG_MAX_PATH_LENGTH] = {0};
    strcpy(currentPath, path);
    if (path[pathLen - 1] != '/') {
        strcat(currentPath, "/");
    }
    if (zl_file_exist(currentPath) == 1) {
        return 0;
    }
    zl_printf("makedir > path : %s\n", currentPath);
    size_t i,len = strlen(currentPath);
    for(i=1; i<len; i++) {
        if ('/' == currentPath[i]) {
            currentPath[i] = '\0';
            zl_printf("=== 正在创建文件夹: current= %s \n",currentPath);
            if (access(currentPath, F_OK) != 0) {
                zl_printf("=== 当前文件夹不存在：%s \n", currentPath);
                int code = mkdir(currentPath, 0777);
                zl_printf("=== 创建结果，code: %d \n", code);
                if (code == -1) {
                    return -1;
                }
            }
            currentPath[i] = '/';
        }
    }
    return 0;
}

int zl_file_exist(const char *path) {
    int isExist = 0;
    if (NULL != path && strnlen(path, 1) > 0) {
        if (access(path, F_OK) == 0) {
            isExist = 1;
        }
    }
    return isExist;
}
