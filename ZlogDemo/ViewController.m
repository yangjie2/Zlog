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
    [self performSelector:@selector(kkk)];
}

- (IBAction)writeLog:(id)sender {
    NSString *text = self.textView.text;
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t start = mach_absolute_time();
    for (int i=0; i<1; i++) {
        LOG_INFO("Login", @"=== 放假了睡觉啊风景Klas机路附近的萨减肥啊时间来康。");
        LOG_ERROR("Login", @"=== 放假了睡觉啊风景Klas机路附近的萨减肥啊时间来康。");
//        NSLog(@"=== 放假了睡觉啊风景Klas机路附近的萨减肥啊时间来康。");
//        NSLog(@"=== 放假了睡觉啊风景Klas机路附近的萨减肥啊时间来康。");
    }
    self.textView.text = @"";
    uint64_t end = mach_absolute_time();
    uint64_t elapsed = end - start;
    uint64_t nanos = elapsed * info.numer / info.denom;
    CGFloat time = (CGFloat)nanos / NSEC_PER_SEC;
    NSLog(@"总用时： %@",@(time));
}



@end
