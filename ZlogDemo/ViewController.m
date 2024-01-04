//
//  ViewController.m
//  ZlogDemo
//
//  Created by admin on 2020/10/30.
//

#import "ViewController.h"
#import "Zlog.h"
#import <mach/mach_time.h>
#import "FLEXManager.h"

@interface ViewController ()
- (IBAction)writeLog:(id)sender;
- (IBAction)makeCrash:(id)sender;
@property (weak, nonatomic) IBOutlet UITextView *textView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [[FLEXManager sharedManager] showExplorer];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    [[FLEXManager sharedManager] showExplorer];
}


- (IBAction)makeCrash:(id)sender {
//    exit(0);
}

- (IBAction)writeLog:(id)sender {
    NSString *text = self.textView.text;
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t start = mach_absolute_time();
    for (int i=0; i<100000; i++) {
        @autoreleasepool {
            NSString *log = [NSString stringWithFormat:@"%@ %@",@(i), text];
            LOG_INFO("=== 当前文件夹不存在：/var/mobile/Containers/Data/Application/EB8F6491-A3AA-4B6A-8E40-35032C5D4742/Documents/log 放假了睡觉啊风景Klas机路附近的萨减肥啊时间来康复苏打粉离开家啊圣诞快乐分安德森饭撒地方 看了放声大哭福师大；克己复礼看啊时代峰峻卡上的纠纷。", log);
        }
    }
    self.textView.text = @"";
    uint64_t end = mach_absolute_time();
    uint64_t elapsed = end - start;
    uint64_t nanos = elapsed * info.numer / info.denom;
    CGFloat time = (CGFloat)nanos / NSEC_PER_SEC;
    NSLog(@"总用时： %@",@(time));
}



@end
