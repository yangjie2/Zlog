//
//  zlutils.c
//  czlog
//
//  Created by admin on 2020/10/28.
//

#include "zlutils.h"
#include <stdio.h>
#include <stdarg.h>

static int zlog_is_debug = 1;

int zl_printf(const char *fmt, ...) {
    int cnt = 0;
    if (zlog_is_debug) {
        va_list argptr;
        va_start(argptr, fmt);
        cnt = vprintf(fmt, argptr);
        va_end(argptr);
    }
    return (cnt);
}

void zl_set_debug(int debug) {
    zlog_is_debug = debug;
}
