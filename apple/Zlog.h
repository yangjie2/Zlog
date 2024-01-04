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

///是否允许控制台打印
+ (void)enableConsole:(BOOL)isenable;

//写入
+ (void)flush;

+ (void)close;

+ (void)logWithLevel:(ZLogLevel)level tag:(const char*)tag fileName:(const char *)fileName lineNum:(int)lineNum funcName:(const char *)funcName msg:(NSString *)msg;

@end



//private
#define LogInternal(level, module, file, line, func, prefix, format, ...) \
do { \
    NSString *aMessage = [NSString stringWithFormat:@"%@%@", prefix, [NSString stringWithFormat:format, ##__VA_ARGS__, nil]]; \
    [Zlog logWithLevel:level tag:module fileName:file lineNum:line funcName:func msg:aMessage]; \
} while(0)
