//
//  mutex.c
//  Zlog
//
//  Created by admin on 2020/11/4.
//


#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <errno.h>
#include <stdint.h>
#include <pthread.h>

class Mutex {
  public:
    Mutex(bool _recursive = false)
        : magic_(reinterpret_cast<uintptr_t>(this)), mutex_(), attr_() {
        //禁止重复加锁
        int ret = pthread_mutexattr_init(&attr_);
        ret = pthread_mutexattr_settype(&attr_, _recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK);
        ret = pthread_mutex_init(&mutex_, &attr_);
    }

    ~Mutex() {
        magic_ = 0;
        int ret = pthread_mutex_destroy(&mutex_);
        ret = pthread_mutexattr_destroy(&attr_);
    }

    bool lock() {
        if (reinterpret_cast<uintptr_t>(this) != magic_) return false;
        int ret = pthread_mutex_lock(&mutex_);
        return 0 == ret;
    }

    bool unlock() {
        int ret = pthread_mutex_unlock(&mutex_);
        return 0 == ret;
    }

    bool trylock() {
        if (reinterpret_cast<uintptr_t>(this) != magic_) return false;
        int ret = pthread_mutex_trylock(&mutex_);
        if (EBUSY == ret) return false;
        return 0 == ret;
    }
    
    bool islocked() {
        int ret = pthread_mutex_trylock(&mutex_);
        if (0 == ret) unlock();
        return 0 != ret;
    }

    pthread_mutex_t& internal() { return mutex_; }

  private:
    Mutex(const Mutex&);
    Mutex& operator = (const Mutex&);
    
  private:
    uintptr_t    magic_;  // Dangling pointer will dead lock, so check it!!!
    pthread_mutex_t mutex_;
    pthread_mutexattr_t attr_;
};

#endif
