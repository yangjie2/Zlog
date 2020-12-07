//
//  zlutils.h
//  czlog
//
//  Created by admin on 2020/10/28.
//

#ifndef zlutils_h
#define zlutils_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 控制台打印
 */
int zl_printf(const char *fmt, ...);

/**
 * 设置编译环境
 */
void zl_set_debug(int debug);


#ifdef __cplusplus
}
#endif


#endif /* zlutils_h */
