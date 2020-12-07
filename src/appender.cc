//
//  appender.c
//  czlog
//
//  Created by admin on 2020/10/28.
//

#include "appender.h"

#ifdef _WIN32
#include <windows.h>
#define PRIdMAX "lld"
#define snprintf _snprintf
#define fileno _fileno
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sys/mount.h>
#endif

#ifdef __APPLE__
// apple 平台特性
#endif

#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <zlib.h>
#include <string>
#include <algorithm>
#include <vector>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include "zlutils.h"
#include "dir_util.h"
#include "time_util.h"
#include "dir_util.h"
#include "scopelock.cc"
#include "formater.h"

//TODO: 设置文件最大阈值，然后分文件，目前并没有实现，一天的日志量写入到一个文件中
static const uint64_t ZLOG_MAX_ONE_LOGFILE_SIZE = 0; // 0, will not split log file.

#define ZLOG_EXT "zlog" //扩展名

Appender::Appender(const char* dir, const char* cache_dir, const char* nameprefix, const char* pub_key) {
    _level = kLevelInfo;
    _log_dir = dir;
    _cache_dir = cache_dir;
    _nameprefix = nameprefix;
    _is_appender_close = true;
    _log_buffer = NULL;
    _logfile = NULL;
    _open_filetime = 0;
    _mmap_filepath = "";
}

Appender::~Appender() {
    Appender::reset();
}

void Appender::reset() {
    closelogbuffer();
    _level = kLevelNone;
    _log_dir = "";
    _cache_dir = "";
    _pub_key = "";
    _is_appender_close = true;
    _logfile = NULL;
    _open_filetime = 0;
    _last_tick = 0;
    _last_time = 0;
    _last_file_path[0] = '\0';
    _mmap_filepath = "";
    _use_mmap = false;
}

void Appender::setloglevel(ZlogLevel level) {
    if (level == _level) {
        return;
    }
    _level = level;
}

ZlogLevel Appender::loglevel() {
    return _level;
}

void Appender::open() {
    assert(!_log_dir.empty());
    assert(!_nameprefix.empty());
        
    if (_is_appender_close == false) {
        zl_printf("appender has already been opened. _log_dir:%s _nameprefix:%s", _log_dir.c_str(), _nameprefix.c_str());
        return;
    }
    
    //create dir
    zl_makedir(_log_dir.c_str());
    if (!_cache_dir.empty() && _log_dir != _cache_dir) {
        zl_makedir(_cache_dir.c_str());
    }
        
    //mmap
    if (!make_logbuffer()) {
        return;
    }
    
    //这里会进行压缩加密处理
    AutoBuffer buffer;
    _log_buffer->Flush(buffer);
    
    ScopedLock lock(_logfile_mutex);
    _is_appender_close = false;
    lock.unlock();
    
    char mark_info[512] = {0};
    get_mark_str(mark_info, sizeof(mark_info));
    
    if (buffer.Ptr()) {
        //如果有mmap缓存，先把缓存写入文件
        writefile_format("\n~~~~~ begin of mmap ~~~~~\n"); //使用压缩加密接口
        if (open_logfile()) {
            writefile(buffer.Ptr(), buffer.Length()); //因为上面已经进行压缩加密，所以这里调用非压缩加密接口,写入 mmap 中缓存的数据到文件
        }
        writefile_format("~~~~~ end of mmap ~~~~~%s\n", mark_info);
    }
    
    char appender_info[728] = {0};
    snprintf(appender_info, sizeof(appender_info), "^^^^^^^^^^[" __DATE__ "]^^^[" __TIME__ "]^^^^^^^^^^%s\n", mark_info);
    
    writefile_format(appender_info);
        
#ifdef __APPLE__
    //TODO: 设置路径的属性 NSFileProtectionNone , apple 平台下实现
//setAttributesAtPath(const char* path)
#endif
    
}

void Appender::flush() {
    ScopedLock lock_buffer(_logbuffer_mutex);
    if (NULL == _log_buffer || _is_appender_close) {
        return;
    }
    AutoBuffer tmp;
    _log_buffer->Flush(tmp);
    lock_buffer.unlock();
    
    if (NULL != tmp.Ptr()) {
        writefile_fragment(tmp.Ptr(), tmp.Length());
    }
}

void Appender::close() {
    if (_is_appender_close) {
        return;
    }
    char mark_info[512] = {0};
    get_mark_str(mark_info, 512);
    char appender_info[728] = {0};
    snprintf(appender_info, sizeof(appender_info), "$$$$$$$$$$[" __DATE__ "]$$$[" __TIME__ "]$$$$$$$$$$%s\n", mark_info);
    writefile_format(appender_info);
    
    _is_appender_close = true;
    
    flush();
    
    ScopedLock buffer_lock(_logbuffer_mutex);
    closelogbuffer();
    buffer_lock.unlock();
    
    //关闭日志文件
    ScopedLock lock(_logfile_mutex);
    closelogfile();
}

void Appender::writelog(const ZLogBasicInfo* info, const char* log) {
    assert(NULL != info);
    assert(NULL != log);
    ScopedLock lock(_logbuffer_mutex);
    
    if (_is_appender_close || NULL == _log_buffer) {
        return;
    }
    
    //TODO: ============ 最耗时操作：access() 函数
    //再次检查日志文件目录是否存在
    if (zl_file_exist(_log_dir.c_str()) == 0) {
        zl_makedir(_log_dir.c_str());
    }
    //再次检查mmap文件是否存在
    if (_use_mmap && zl_file_exist(_mmap_filepath.c_str()) == 0) {
        //先关闭
        closemmapfile();
        //重新打开
        if (!make_logbuffer()) {
            return;
        }
    }
    // ============ 最耗时部分
    
    
    //format 转化为需要的格式
    std::string format = log_formater(info, log);
    const char *logdata = format.c_str(); size_t len = format.length();
    
    if (!_log_buffer->Write(logdata, len)) {
        return;
    }
    //判断buffer 数据长度，大于阈值马上写入文件
    if (_log_buffer->GetData().Length() >= ZLOG_MMAP_LENGTH*0.35 || (NULL != log && info->level == kLevelFatal)) {
        flush();
    }
}





//MARK: - private
bool Appender::make_logbuffer() {
    char mmap_file_path[512] = {0};
    snprintf(mmap_file_path, sizeof(mmap_file_path), "%s/%s.mmap3", _cache_dir.empty()?_log_dir.c_str():_cache_dir.c_str(), _nameprefix.c_str());
    //开启mmap
    if (mmap_file_open(mmap_file_path, &_mmap_f)) {
        _use_mmap = true;
        _mmap_filepath = mmap_file_path;
        _log_buffer = new ZLogBuffer(_mmap_f.mmap_ptr, ZLOG_MMAP_LENGTH, true, _pub_key.c_str());
    }else {
        _use_mmap = false;
        _mmap_filepath = "";
        //开启mmap失败，使用内存做缓存
        char* buffer = new char[ZLOG_MMAP_LENGTH];
        _log_buffer = new ZLogBuffer(buffer, ZLOG_MMAP_LENGTH, true, _pub_key.c_str());
    }
    
    if (NULL == _log_buffer->GetData().Ptr()) {
        closelogbuffer();
        return false;
    }
    return true;
}

bool Appender::open_logfile() {
    if (_log_dir.empty()) {
        return false;
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    //顺序 1
    if (NULL != _logfile) {
        time_t sec = tv.tv_sec;
        tm tcur = *localtime((const time_t*)&sec);
        tm filetm = *localtime(&_open_filetime);
        bool istoday = filetm.tm_year == tcur.tm_year && filetm.tm_mon == tcur.tm_mon && filetm.tm_mday == tcur.tm_mday;
        if (istoday && ZLOG_MAX_ONE_LOGFILE_SIZE == 0) {
            return true;
        }
        closelogfile();
    }
    //顺序 2
    uint64_t now_tick = system_mach_time();
    time_t now_time = tv.tv_sec;
    _open_filetime = tv.tv_sec;
    
    //顺序 3
    char logfilepath[1024] = {0};
    make_logfilepath(tv, _log_dir, _nameprefix, logfilepath, 1024);
    
    //当前时间小于上次记录的时间，则已经关闭的文件从新打开
    if (now_time < _last_time) {
        _logfile = fopen(_last_file_path, "ab"); //追加打开一个二进制文件，并在文件末尾写数据
        if (NULL == _logfile) {
            zl_printf("open file error:%d %s, path:%s", errno, strerror(errno), _last_file_path);
        }
        return NULL != _logfile;
    }
    
    _logfile = fopen(logfilepath, "ab");
    if (NULL == _logfile) {
        zl_printf("open file error:%d %s, path:%s", errno, strerror(errno), logfilepath);
        return false;
    }

    if (0 != _last_time && (now_time - _last_time) > (time_t)((now_tick - _last_tick) / 1000 + 300)) {
        struct tm tm_tmp = *localtime((const time_t*)&_last_time);
        char last_time_str[64] = {0};
        strftime(last_time_str, sizeof(last_time_str), "%Y-%m-%d %z %H:%M:%S", &tm_tmp);

        tm_tmp = *localtime((const time_t*)&now_time);
        char now_time_str[64] = {0};
        strftime(now_time_str, sizeof(now_time_str), "%Y-%m-%d %z %H:%M:%S", &tm_tmp);

        char log[1024] = {0};
        snprintf(log, sizeof(log), "[F][ last log file:%s from %s to %s, time_diff:%ld, tick_diff:%" PRIu64 "\n", _last_file_path, last_time_str, now_time_str, now_time-_last_time, now_tick-_last_tick);

        AutoBuffer tmp_buff;
        _log_buffer->Write(log, strnlen(log, sizeof(log)), tmp_buff);
        writefile(tmp_buff.Ptr(), tmp_buff.Length());
    }
    
    memcpy(_last_file_path, logfilepath, sizeof(_last_file_path));
    _last_tick = now_tick;
    _last_time = now_time;
    
    return NULL != _logfile;
}

void Appender::make_logfilepath(const timeval&tv, const std::string& logdir, const std::string& nameprefix, char* filepath, unsigned int len) {
    long index = 0;
    std::string filenameprefix = make_nameprefix(tv, nameprefix);
    if (ZLOG_MAX_ONE_LOGFILE_SIZE > 0) {
        index = get_next_fileindex(filenameprefix, ZLOG_EXT);
    }
    
    std::string temppath = logdir;
    temppath += "/";
    temppath += filenameprefix;
    
    if (index > 0) {
        char temp[24] = {0};
        snprintf(temp, 24, "_%ld", index);
        temppath += temp;
    }
    temppath += ".";
    temppath += ZLOG_EXT;
    strncpy(filepath, temppath.c_str(), len - 1);
    filepath[len - 1] = '\0';
}

std::string Appender::make_nameprefix(const timeval& tv, const std::string& nameprefix) {
    time_t sec = tv.tv_sec;
    tm tcur = *localtime((const time_t*)&sec);
    
    char temp [64] = {0};
    snprintf(temp, 64, "_%d%02d%02d", 1900 + tcur.tm_year, 1 + tcur.tm_mon, tcur.tm_mday);
    
    std::string filenameprefix = nameprefix;
    filenameprefix += temp;
    
    return filenameprefix;
}

long Appender::get_next_fileindex(const std::string& logdir, const std::string& filenameprefix) {
    return 0;
}

//void Appender::get_all_filenames_with_filenameprefix(const std::string& logdir, const std::string& filenameprefix, std::vector<std::string>& vector) {
//
//}


//仅调用打印内部日志
void Appender::writefile_format(const char* format, ...) {
    if (NULL == format) {
        return;
    }
    char logdata[4096] = {0};
    va_list args;
    va_start(args, format);
    int n = vsnprintf(logdata, 4096, format, args);
    va_end(args);
    if (n <= 0) {
        return;
    }
    AutoBuffer tmp_buff;
    _log_buffer->Write(logdata, n, tmp_buff);
    writefile(tmp_buff.Ptr(), tmp_buff.Length());
}

//不压缩加密
void Appender::writefile_fragment(void* data, size_t len) {
    if (NULL == data || 0 == len) {
        return;
    }
        
    int size = ZLOG_FRAGMENT_SIZE;
    if (len <= size) {
        writefile(data, len);
        return;
    }
    unsigned long times = len / size;
    unsigned int remain_len = len % size;
    char *temp = (char *)data;
    
    for (int i = 0; i < times; i++) {
        writefile(temp, size);
        temp += size;
    }
    if (remain_len > 0) {
        writefile(temp, remain_len);
    }
}

//写入数据到文件，不会将数据压缩加密
bool Appender::writefile(const void* data, size_t len) {
    if (NULL ==data || 0 == len) {
        return false;
    }
    ScopedLock lock_file(_logfile_mutex);
    if (!open_logfile()) {
        return false;
    }
    long before_len = ftell(_logfile);
    if (before_len < 0) return false;
    
    if (1 != fwrite(data, len, 1, _logfile)) {
        int errcode = ferror(_logfile);
        ftruncate(fileno(_logfile), before_len); //参数fd指定的文件大小改为参数length指定的大小,如果原来的文件大小比参数length大，则超过的部分会被删去
        fseek(_logfile, 0, SEEK_END);

        char err_log[256] = {0};
        snprintf(err_log, sizeof(err_log), "\nwrite file error:%d\n", errcode);
        
        //这里压缩加密的仅仅是这行错误日志，然后会写到文件
        AutoBuffer tmp_buff;
        _log_buffer->Write(err_log, strnlen(err_log, sizeof(err_log)), tmp_buff);
        fwrite(tmp_buff.Ptr(), tmp_buff.Length(), 1, _logfile);
        return false;
    }
    fflush(_logfile);
    return true;
}

void Appender::get_mark_str(char* info, size_t len) {
    struct timeval tv;
    gettimeofday(&tv, 0);
    time_t sec = tv.tv_sec;
    struct tm tm_tmp = *localtime((const time_t*)&sec);
    char tmp_time[64] = {0};
    strftime(tmp_time, sizeof(tmp_time), "%Y-%m-%d %z %H:%M:%S", &tm_tmp);
    snprintf(info, len, "[%s]", tmp_time);
}

void Appender::closelogfile() {
    if (NULL == _logfile) return;
    _open_filetime = 0;
    fclose(_logfile);
    _logfile = NULL;
}

void Appender::closelogbuffer() {
    if (_use_mmap) {
        closemmapfile();
    }else {
        delete[] (char*)((_log_buffer->GetData()).Ptr());
    }
    delete _log_buffer;
    _log_buffer = NULL;
}

void Appender::closemmapfile() {
    if (_use_mmap && mmap_file_close(&_mmap_f)) {
        _use_mmap = false;
        _mmap_filepath = "";
    }
}
