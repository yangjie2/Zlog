//
//  dir_util.h
//  czlog
//
//  Created by admin on 2020/10/28.
//

#ifndef _DIR_UTIL_H_
#define _DIR_UTIL_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//根据绝对路径创建目录
//注意：一定是绝对路径
int zl_makedir(const char *path);

//文件或者文件目录是否存在
int zl_file_exist(const char *path);

#ifdef __cplusplus
}
#endif


#endif /* _DIR_UTIL_H_ */
