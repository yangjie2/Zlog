//
//  PWLogFileUtil.h
//  IPC360Pro
//
//  Created by 王魏 on 2019/12/11.
//  Copyright © 2019 PUWELL. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface PWLogFileUtil : NSObject

+ (NSArray *)getLogPaths;
/**
 打印重定向到文件中
 */
+ (void)redirectLog2File;

@end

NS_ASSUME_NONNULL_END
