//
//  extract_util.h
//  Zlog
//
//  Created by yangjie on 2020/11/3.
//

#ifndef _EXTRACT_UTIL_H_
#define _EXTRACT_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

const char* extractFileName(const char* _path);

void extractFunctionName(const char* _func, char* _func_ret, int _len);

#ifdef __cplusplus
}
#endif

#endif /* _EXTRACT_UTIL_H_ */
