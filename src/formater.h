//
//  formater.c
//  czlog
//
//  Created by admin on 2020/10/30.
//

#ifndef _FORMATER_H_
#define _FORMATER_H_

#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <algorithm>
#include "ptrbuffer.h"
#include "appender.h"
#include "extract_util.h"

#ifdef _WIN32
#define PRIdMAX "lld"
#define snprintf _snprintf
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif

std::string log_formater(const ZLogBasicInfo* info, const char* logmessage) {
    static const char* levelStrings[] = {
        "V",
        "D",  // debug
        "I",  // info
        "W",  // warn
        "E",  // error
        "F"  // fatal
    };
    std::string format = "";
    if (NULL != info) {
        const char* filename = extractFileName(info->filename);
        char funcname [128] = {0};
        extractFunctionName(info->func_name, funcname, sizeof(funcname));
        
        char temp_time[64] = {0};
        if (0 != info->timeval.tv_sec) {
            time_t sec = info->timeval.tv_sec;
            tm tm = *localtime((const time_t*)&sec);
            #ifdef ANDROID
            snprintf(temp_time, sizeof(temp_time), "%d-%02d-%02d %+.1f %02d:%02d:%02d.%.3d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                                 tm.tm_gmtoff / 3600.0, tm.tm_hour, tm.tm_min, tm.tm_sec, info->timeval.tv_usec / 1000);
            #elif _WIN32
            snprintf(temp_time, sizeof(temp_time), "%d-%02d-%02d %+.1f %02d:%02d:%02d.%.3d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                                 (-_timezone) / 3600.0, tm.tm_hour, tm.tm_min, tm.tm_sec, info->timeval.tv_usec / 1000);
            #else
            snprintf(temp_time, 64, "%d-%02d-%02d %+.1f %02d:%02d:%02d.%.3d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_gmtoff / 3600.0, tm.tm_hour, tm.tm_min, tm.tm_sec, info->timeval.tv_usec / 1000);

            #endif
        }

        char temp_info[1024] = {0};
        int ret = snprintf(temp_info, 1024, "[%s][%s][%" PRIdMAX ", %" PRIdMAX "][%s][%s][%s, %s, %d][",
                           logmessage ? levelStrings[info->level] : levelStrings[kLevelFatal], temp_time,
                           info->pid, info->tid, info->tid == info->main_tid?"*":"-", info->tag ? info->tag : "",
                           filename, funcname, info->line);

        assert(0 <= ret);
        std::string info_str = temp_info;
        format += info_str;
    }
    if (NULL != logmessage) {
        format = format + logmessage + "]";
    }
    
    //换行符
    format += "\n";
    return format;
}

#endif