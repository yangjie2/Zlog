//
//  typedefine.h
//  czlog
//
//  Created by admin on 2020/11/2.
//

#ifndef _TYPEDEFINE_H_
#define _TYPEDEFINE_H_

#include <sys/time.h>
#include <stdint.h>

typedef enum {
    kLevelVerbose = 0,
    kLevelDebug,
    kLevelInfo,
    kLevelWarn,
    kLevelError,
    kLevelFatal,
    kLevelNone,
} ZlogLevel;

typedef struct Log_basic_info_struct {
    ZlogLevel level;
    const char* tag;
    const char* filename;
    const char* func_name;
    int line;

    struct timeval timeval;
    intmax_t pid; //进程id
    intmax_t tid; //线程id
    intmax_t main_tid;//主线程id
} ZLogBasicInfo;


#endif /* _TYPEDEFINE_H_ */
