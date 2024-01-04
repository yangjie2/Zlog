//
//  czlog.c
//  czlog
//
//  Created by admin on 2020/11/2.
//

#include "czlog.h"
#include "appender.h"

#ifndef VARIABLE_IS_NOT_USED
#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif
#endif

__inline int __zlog_run_atexit(void (*func)(void)) { return atexit(func);}
#define ZLOG_RUN_EXIT(func) VARIABLE_IS_NOT_USED static int __anonymous_run_variable_exit_##func = __zlog_run_atexit(func)


static Appender *appender = NULL;
void c_logopen(const char* dir, const char* nameprefix, const char* pub_key) {
    if (NULL == appender) {
        appender = new Appender(dir, nameprefix, pub_key);
    }
    appender->open();
    ZLOG_RUN_EXIT(c_logclose);
}

void c_setloglevel(ZlogLevel level) {
    appender->setloglevel(level);
}

ZlogLevel c_loglevel() {
    return appender->loglevel();
}

void c_logflush() {
    appender->flush();
}

void c_logclose() {
    printf("======= 关闭.");
    appender->close();
}

void c_writelog(const ZLogBasicInfo *info, const char *msg) {
    appender->writelog(info, msg);
}
