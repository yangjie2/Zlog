//
//  Zlog.h
//  ZlogDemo
//
//  Created by yangjie on 2020/11/1.
//

#import <Foundation/Foundation.h>

#define __FILENAME__ (strrchr(__FILE__,'/')+1)

typedef enum {
    kVerbose = 0,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kFatal,
    kNone,
} ZLogLevel;


#define LOG_ERROR(module, format, ...) LogInternal(kError, module, __FILENAME__, __LINE__, __FUNCTION__, @"Error: ", format, ##__VA_ARGS__)
#define LOG_WARNING(module, format, ...) LogInternal(kWarn, module, __FILENAME__, __LINE__, __FUNCTION__, @"Warning: ", format, ##__VA_ARGS__)
#define LOG_INFO(module, format, ...) LogInternal(kInfo, module, __FILENAME__, __LINE__, __FUNCTION__, @"Info: ", format, ##__VA_ARGS__)
#define LOG_DEBUG(module, format, ...) LogInternal(kDebug, module, __FILENAME__, __LINE__, __FUNCTION__, @"Debug: ", format, ##__VA_ARGS__)

@interface Zlog : NSObject
//初始化打开文件
+ (void)open:(NSString *)dir nameprefix:(NSString *)nameprefix;

+ (void)setLoglevel:(ZLogLevel)level;

///设置是否是DEBUG模式  1:DEBUG  0:RELEASE
///DEBUG 模式下，会在控制台打印日志
+ (void)setDebugEnv:(int)debug;

//清空缓存，并写入到文件
+ (void)flush;

///应用退出/终止的时候调用
///当应用程序被杀掉（比如被用户强制退出）或者崩溃（crash）时，操作系统的内核会负责清理进程占用的资源，包括打开的文件描述符、分配的内存以及建立的内存映射。因此，即使应用程序在没有显式解除映射（通过 munmap 调用）的情况下终止，操作系统也会自动解除映射并释放相应资源。
+ (void)close;

///打印日志
+ (void)logWithLevel:(ZLogLevel)level tag:(const char*)tag fileName:(const char *)fileName lineNum:(int)lineNum funcName:(const char *)funcName msg:(NSString *)msg;

@end



//private
#define LogInternal(level, module, file, line, func, prefix, format, ...) \
do { \
    NSString *aMessage = [NSString stringWithFormat:@"%@%@", prefix, [NSString stringWithFormat:format, ##__VA_ARGS__, nil]]; \
    [Zlog logWithLevel:level tag:module.UTF8String fileName:file lineNum:line funcName:func msg:aMessage]; \
} while(0)
