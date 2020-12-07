//
//  time_util.c
//  czlog
//
//  Created by admin on 2020/10/29.
//

#include "time_util.h"

#include <sys/time.h>
#include <stdint.h>

#ifdef ANDROID

#include <time.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <stdatomic.h>
#include "android_alarm.h"
#include <fcntl.h>
#include <unistd.h>

uint64_t system_mach_time() {
    static int s_fd = -1;
    static int errcode  = 0;
    if (s_fd == -1 && EACCES != errcode) {
        int fd = open("/dev/alarm", O_RDONLY);
        if (-1 == fd) errcode = errno;
        atomic_int x = ATOMIC_VAR_INIT(s_fd);
        int expect = -1;
        if (!atomic_compare_exchange_strong(&x, &expect, fd)) {
            if (fd >=0) close(fd);
        }
        s_fd = atomic_load(&x);
    }

    struct timespec ts;
    int result = ioctl(s_fd, ANDROID_ALARM_GET_TIME(ANDROID_ALARM_ELAPSED_REALTIME), &ts);
    if (result != 0) {
        // XXX: there was an error, probably because the driver didn't
        // exist ... this should return
        // a real error, like an exception!
        clock_gettime(CLOCK_BOOTTIME, &ts);
    }
    return (uint64_t)ts.tv_sec*1000 + (uint64_t)ts.tv_nsec/1000000;
}

#elif defined __APPLE__

#include <mach/mach_time.h>

uint64_t system_mach_time() {
    static mach_timebase_info_data_t timebase_info = {0};
    
    // Convert to nanoseconds - if this is the first time we've run, get the timebase.
    if (timebase_info.denom == 0 ) {
        (void) mach_timebase_info(&timebase_info);
    }
    
    // Convert the mach time to milliseconds
    uint64_t mach_time = mach_absolute_time();
    uint64_t millis = (mach_time * timebase_info.numer) / (timebase_info.denom * 1000000);
    return millis;
}

#elif defined Q_OS_BLACKBERRY

#include <time.h>

uint64_t system_mach_time() {//todoyy
    struct timespec ts;
    if (0==clock_gettime(CLOCK_MONOTONIC, &ts)){
        return (ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000);
    }
    return 0;
}

#elif defined __linux__

#include <time.h>

uint64_t system_mach_time() {//todoyy
    struct timespec ts;
    if (0==clock_gettime(CLOCK_MONOTONIC, &ts)){
        return (ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000);
    }
    return 0;
}

#elif defined _WIN32

#include <windows.h>

uint64_t system_mach_time() {
    return GetTickCount();
}

#elif WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP || UWP

#include "unistd.h"
#include <stdint.h>

uint64_t system_mach_time() {
    return GetTickCount64();
}

#else
#error "not support"
#endif

uint64_t get_timestamp() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}
