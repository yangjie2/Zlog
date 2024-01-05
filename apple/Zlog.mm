//
//  Zlog.m
//  ZlogDemo
//
//  Created by yangjie on 2020/11/1.
//

#import "Zlog.h"
#include "czlog.h"
#include "zlutils.h"

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#define ZLogAppWillEnterForegroundNotification UIApplicationWillEnterForegroundNotification
#define ZLogAppDidEnterBackgroundNotification UIApplicationDidEnterBackgroundNotification
#define ZLogAppWillTerminateNotification UIApplicationWillTerminateNotification
#else
#import <Cocoa/Cocoa.h>
#define ZLogAppWillEnterForegroundNotification NSApplicationWillBecomeActiveNotification
#define ZLogAppDidEnterBackgroundNotification NSApplicationDidResignActiveNotification
#define ZLogAppWillTerminateNotification NSApplicationWillTerminateNotification
#endif

//随机整数，标志每次启动后的进程
static NSUInteger ZLOG_PROCESS_ID = (arc4random() % 1000000) + 100000;

@interface Zlog ()
@property (nonatomic, strong) NSLock *lock;
@end

@implementation Zlog

+ (instancetype)shared {
    static Zlog *instance = nil;
    static dispatch_once_t token;
    dispatch_once(&token, ^{
        instance = [[Zlog alloc] initPrivate];
    });
    return instance;
}

- (instancetype)initPrivate {
    if (self = [super init]) {
        _lock = [[NSLock alloc] init];
        [self addObserver];
    }
    return self;
}

- (void)addObserver {
    // App Extension
    if ( [[[NSBundle mainBundle] bundlePath] hasSuffix:@".appex"] ) {
        return ;
    }
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillEnterForeground) name:ZLogAppWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appDidEnterBackground) name:ZLogAppDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appWillTerminate) name:ZLogAppWillTerminateNotification object:nil];
}

- (void)appDidEnterBackground {
    [Zlog flush];
}

- (void)appWillEnterForeground {
    [Zlog flush];
}

- (void)appWillTerminate {
    [Zlog flush];
    [Zlog close];
}


//MARK: - public
+ (void)open:(NSString *)dir nameprefix:(NSString *)nameprefix {
    [Zlog shared];
    c_logopen(dir.UTF8String, nameprefix.UTF8String, "");
}

+ (void)setLoglevel:(ZLogLevel)level {
    c_setloglevel((ZlogLevel)level);
}

+ (void)setDebugEnv:(int)debug {
    zl_set_debug(debug);
}

+ (void)flush {
    c_logflush();
}

+ (void)close {
    c_logclose();
}

+ (void)logWithLevel:(ZLogLevel)level tag:(const char*)tag fileName:(const char *)fileName lineNum:(int)lineNum funcName:(const char *)funcName msg:(NSString *)msg {
    if (level < (ZLogLevel)c_loglevel()) {
        return;
    }
    if (!msg) {
        return;
    }
    ZLogBasicInfo info;
    info.level = (ZlogLevel)level;
    info.tag = tag;
    info.filename = fileName;
    info.func_name = funcName;
    info.line = lineNum;
    struct timeval tv;
    gettimeofday(&tv, 0);
    info.timeval = tv;
    info.pid = ZLOG_PROCESS_ID;
    info.tid = (uintptr_t)[NSThread currentThread];
    info.main_tid = (uintptr_t)[NSThread mainThread];
    c_writelog(&info, msg.UTF8String);
}


@end
