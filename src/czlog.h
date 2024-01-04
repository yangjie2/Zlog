//
//  czlog.h
//  czlog
//
//  Created by admin on 2020/11/2.
//

#ifndef _CZLOG_H_
#define _CZLOG_H_

#include "typedefine.h"
    
void c_logopen(const char* dir, const char* nameprefix, const char* pub_key);
    
void c_setloglevel(ZlogLevel level);

ZlogLevel c_loglevel();

void c_logflush();

void c_logclose();

void c_writelog(const ZLogBasicInfo *info, const char *msg);


#endif /* _CZLOG_H_ */
