//
//  zl_buffer.h
//  czlog
//
//  Created by admin on 2020/10/28.
//

#ifndef _ZL_BUFFER_H_
#define _ZL_BUFFER_H_

#include <zlib.h>
#include <string.h>
#include <stdint.h>
#include <iosfwd>
#include "ptrbuffer.h"
#include "autobuffer.h"


class ZLogBuffer {
public:
    ZLogBuffer(void* _pbuffer, size_t _len, bool _is_compress, const char* _pubkey);
    ~ZLogBuffer();
    
public:
    static bool GetPeriodLogs(const char* _log_path, int _begin_hour, int _end_hour, unsigned long& _begin_pos, unsigned long& _end_pos, std::string& _err_msg);
    
public:
    PtrBuffer& GetData();
    
    void Flush(AutoBuffer& _buff);
    bool Write(const void* _data, size_t _inputlen, AutoBuffer& _out_buff);
    bool Write(const void* _data, size_t _length);

private:
    
    bool __Reset();
    void __Flush();
    void __Clear();
    
    void __Fix();

private:
    PtrBuffer buff_;
    bool is_compress_;
    z_stream cstream_;
    
    class ZLogCrypt* log_crypt_;
    size_t remain_nocrypt_len_;
    
};

#endif /* _ZL_BUFFER_H_ */
