//
//  AppDelegate.m
//  ZlogDemo
//
//  Created by admin on 2020/10/30.
//

#import "AppDelegate.h"
#import <Foundation/Foundation.h>
#import <CrashReporter/CrashReporter.h>
#import "Zlog.h"
#include <sys/xattr.h>
#import "PWLogFileUtil.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    NSString* logPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0] stringByAppendingString:@"/log"];
//    // set do not backup for logpath
    const char* attrName = "com.apple.MobileBackup";
    u_int8_t attrValue = 1;
    setxattr([logPath UTF8String], attrName, &attrValue, sizeof(attrValue), 0, 0);

    [Zlog open:logPath nameprefix:@"1234"];
    [Zlog setLoglevel:kDebug];
    [Zlog setDebugEnv:1];
    
    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [Zlog close];
}



@end
