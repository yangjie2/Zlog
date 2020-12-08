# Zlog

Zlog 是一个适用于 iOS, Android, MacOS 端的高性能日志框架。借鉴微信Xlog以及美团Logan，但对比Xlog，它更加轻量级，无需依赖 boost 库，函数调用、命名以及逻辑更加清晰；对比Logan，它无需在上层保证线程安全，节约了线程调度的时间。

性能测试结果：

一条日志大约200个字符长度，iphone 8，写入 10W 条数据

Xlog 4.6s
Langon 51s
Zlog 3.8s

iOS 端引入方式：

```
pod 'Zlog'
```

安卓端引入方式：

打开 'android' 路径下项目，自行编译 .so 库即可。

# 未完成
1. 单日数据量达到文件大小阈值后，分文件写入
2. 过期自动删除日志文件
