//
//  appender.h
//  czlog
//
//  Created by admin on 2020/10/28.
//

#ifndef _APPENDER_H_
#define _APPENDER_H_


#include <string>
#include "logbuffer.h"
#include "mmap_util.h"
#include "typedefine.h"
#include "mutex.h"

#define ZLOG_FRAGMENT_SIZE (16 * 1024) //数据太大时分片写入


class Appender {

public:
    //所有目录都指的是绝对路径
    //dir: 日志文件目录
    Appender(const char* dir, const char* nameprefix, const char* pub_key);
    ~Appender();
    
public:
    void setloglevel(ZlogLevel level);
    ZlogLevel loglevel();
    void open();
    void flush();
    void close();
    void writelog(const ZLogBasicInfo* info, const char* log); //写日志
    
private:
    void reset();
    void get_mark_str(char* info, size_t len); //产生标记字符
    bool open_logfile(); //log_dir 当前存放日志的目录
    bool make_logbuffer(); //创建log的buffer，mmap或者内存buffer
    void make_logfilepath(const timeval&tv, const std::string& logdir, const std::string& nameprefix, char* filepath, unsigned int len); //创建文件名，返回 filepath 文件全路径
    std::string make_nameprefix(const timeval& tv, const std::string& nameprefix);
    long get_next_fileindex(const std::string& logdir, const std::string& filenameprefix); //一个文件写满后，获取下一个要写入的文件的index
    void get_all_filenames_with_filenameprefix(const std::string& logdir, const std::string& filenameprefix, std::vector<std::string>& vector);
    
    void writefile_format(const char* format, ...); //格式化写入，最大 4096 bytes, 压缩加密
    void writefile_fragment(void* data,  size_t len); //大数据分片写入，不压缩加密
    bool writefile(const void* data, size_t len); //写入数据到文件，不压缩加密
    
    void closelogfile(); //关闭文件
    void closelogbuffer(); //关闭logbuffer
    void closemmapfile(); //关闭mmap文件
    
private:
    ZlogLevel _level;
    std::string _log_dir;
    std::string _nameprefix;
    std::string _pub_key;
    
private:
    bool _is_appender_close;
    ZLogBuffer *_log_buffer;
    FILE* _logfile = NULL; //保存日志的文件
    std::string _mmap_filepath; //mmap文件 xxx.mmap
    time_t _open_filetime = 0;
    bool _use_mmap = false; //正在使用mmap缓存
    MMAP_file _mmap_f;
    
    Mutex _logfile_mutex;
    Mutex _logbuffer_mutex;
    
private:
    time_t _last_time = 0;
    uint64_t _last_tick = 0;
    char _last_file_path[1024] = {0};
};

#endif /* _APPENDER_H_ */
