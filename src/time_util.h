//
//  time_util.h
//  czlog
//
//  Created by admin on 2020/10/29.
//

#ifndef _TIME_UTIL_H_
#define _TIME_UTIL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//返回基于系统启动后的时钟"嘀嗒"数，转换为时间
uint64_t system_mach_time(void);  // ms

//获取当前时间戳
uint64_t get_timestamp(void); //ms

#ifdef __cplusplus
}
#endif

#endif /* _TIME_UTIL_H_ */
