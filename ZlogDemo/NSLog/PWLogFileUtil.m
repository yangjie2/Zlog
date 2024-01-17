//
//  PWLogFileUtil.m
//  IPC360Home
//
//  Created by 王魏 on 2019/12/11.
//  Copyright © 2019 PUWELL. All rights reserved.
//

#import "PWLogFileUtil.h"
#import <mach/mach.h>


#define LOGSIZE    5000*1024 //5000k

NSString * _logPath1 = nil;
NSString * _logPath2 = nil;
NSString * _curWritelogPath = nil;

@implementation PWLogFileUtil
+ (float)cpu_usage
{
    kern_return_t kr;
    task_info_data_t tinfo;
    mach_msg_type_number_t task_info_count;
    
    task_info_count = TASK_INFO_MAX;
    kr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)tinfo, &task_info_count);
    if (kr != KERN_SUCCESS) {
        return -1;
    }
    
    task_basic_info_t      basic_info;
    thread_array_t         thread_list;
    mach_msg_type_number_t thread_count;
    
    thread_info_data_t     thinfo;
    mach_msg_type_number_t thread_info_count;
    
    thread_basic_info_t basic_info_th;
    uint32_t stat_thread = 0; // Mach threads
    
    basic_info = (task_basic_info_t)tinfo;
    
    // get threads in the task
    kr = task_threads(mach_task_self(), &thread_list, &thread_count);
    if (kr != KERN_SUCCESS) {
        return -1;
    }
    if (thread_count > 0)
        stat_thread += thread_count;
    
    long tot_sec = 0;
    long tot_usec = 0;
    float tot_cpu = 0;
    int j;
    
    for (j = 0; j < thread_count; j++)
    {
        thread_info_count = THREAD_INFO_MAX;
        kr = thread_info(thread_list[j], THREAD_BASIC_INFO,
                         (thread_info_t)thinfo, &thread_info_count);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        
        basic_info_th = (thread_basic_info_t)thinfo;
        
        if (!(basic_info_th->flags & TH_FLAGS_IDLE)) {
            tot_sec = tot_sec + basic_info_th->user_time.seconds + basic_info_th->system_time.seconds;
            tot_usec = tot_usec + basic_info_th->user_time.microseconds + basic_info_th->system_time.microseconds;
            tot_cpu = tot_cpu + basic_info_th->cpu_usage / (float)TH_USAGE_SCALE * 100.0;
        }
        
    } // for each thread
    
    kr = vm_deallocate(mach_task_self(), (vm_offset_t)thread_list, thread_count * sizeof(thread_t));
    assert(kr == KERN_SUCCESS);
    
    return tot_cpu;
}

+ (int64_t)memoryUsage {
    int64_t memoryUsageInByte = 0;
    task_vm_info_data_t vmInfo;
    mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
    kern_return_t kernelReturn = task_info(mach_task_self(), TASK_VM_INFO, (task_info_t) &vmInfo, &count);
    if(kernelReturn == KERN_SUCCESS) {
        memoryUsageInByte = (int64_t) vmInfo.phys_footprint;
//        NSLog(@"APP占用内存: %lld字节(B)", memoryUsageInByte);
        int64_t mb = memoryUsageInByte / 1024 / 1024;
    } else {
    }
    return memoryUsageInByte;
}

+ (NSArray *)getLogPaths{
    if (_logPath1 == nil || _logPath2 == nil) {
        [self createLogPath];
    }
    return @[_logPath1,_logPath2];
}
+ (void)createLogPath{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString * docuPath = [paths firstObject];
    NSString * logDirPath = [docuPath stringByAppendingPathComponent:@"log"];
    if (![[NSFileManager defaultManager] fileExistsAtPath:logDirPath]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:logDirPath withIntermediateDirectories:YES attributes:nil error:nil];
    }
    _logPath1 = [logDirPath stringByAppendingPathComponent:@"pw_log1.txt"];
    _logPath2 = [logDirPath stringByAppendingPathComponent:@"pw_log2.txt"];
    
    ///确定需要使用的logPath
    NSFileManager* manager = [NSFileManager defaultManager];
    if([[manager attributesOfItemAtPath:_logPath1 error:nil] fileSize] < LOGSIZE){
        _curWritelogPath = _logPath1;
    }else{
        _curWritelogPath = _logPath2;
    }
}
+ (void)writeLog2File{

//    if (_curWritelogPath == nil || [_curWritelogPath isEqualToString:@""]) {
//        _curWritelogPath = _logPath1;
//    }
    freopen([_curWritelogPath cStringUsingEncoding:NSASCIIStringEncoding], "a+", stderr);
    freopen([_curWritelogPath cStringUsingEncoding:NSASCIIStringEncoding], "a+", stdout);
    NSFileManager* manager = [NSFileManager defaultManager];
    if([[manager attributesOfItemAtPath:_curWritelogPath error:nil] fileSize] >= LOGSIZE){
        //change to another path
        if ([_curWritelogPath isEqualToString:_logPath1]) {
            _curWritelogPath = _logPath2;
        }else{
            _curWritelogPath = _logPath1;
        }
        //clear logpath contents and recreate
        [manager removeItemAtPath:_curWritelogPath error:nil];
        [manager createFileAtPath:_curWritelogPath contents:nil attributes:nil];
        
        //re redict
        freopen([_curWritelogPath cStringUsingEncoding:NSASCIIStringEncoding], "a+", stderr);
        freopen([_curWritelogPath cStringUsingEncoding:NSASCIIStringEncoding], "a+", stdout);
    }
}
+ (void)redirectLog2File
{
    [self createLogPath];//create
    [self writeLog2File];
    dispatch_queue_t aQueue = dispatch_queue_create("com.puwell.redirectLogQueue", NULL);
    dispatch_async(aQueue, ^{
        [[NSThread currentThread] setName:@"redirectLog2File"];
        [NSTimer scheduledTimerWithTimeInterval:1.0f repeats:YES block:^(NSTimer *timer) {
           [self writeLog2File];
//           [self memoryUsage];
        }];
        [[NSRunLoop currentRunLoop] run];
    });
    
    /// For Test
//    dispatch_async(dispatch_get_global_queue(0, 0), ^{
//        int a = 0;
//        while (1) {
//            a++;
//            PWNSLog(@"redirectLog2File:%d",a);
//            usleep(2000);
//        }
//    });

}
@end
