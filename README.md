# Zlog
移动端日志框架

Zlog是一个借鉴微信Xlog以及美团Logan而实现的一个高性能移动端日志框架。对比Xlog，它更加轻量级，无需依赖 boost 库；对比Logan，它无需在上层保证线程安全，节约了线程调度的时间。

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
