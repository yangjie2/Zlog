//
//  scopelock.c
//  Zlog
//
//  Created by admin on 2020/11/4.
//

#include "mutex.h"

template <typename MutexType>
class BaseScopedLock {
  public:
    explicit BaseScopedLock(MutexType& mutex, bool initiallyLocked = true)
        : mutex_(mutex) , islocked_(false) {
        if (!initiallyLocked) return;

        lock();
    }

    ~BaseScopedLock() {
        if (islocked_) unlock();
    }

    bool islocked() const {
        return islocked_;
    }

    void lock() {
        if (!islocked_ && mutex_.lock()) {
            islocked_ = true;
        }
    }

    void unlock() {
        if (islocked_) {
            mutex_.unlock();
            islocked_ = false;
        }
    }

    bool trylock() {
        if (islocked_) return false;

        islocked_ = mutex_.trylock();
        return islocked_;
    }

    MutexType& internal() {
        return mutex_;
    }

  private:
    MutexType& mutex_;
    bool islocked_;
};

typedef BaseScopedLock<Mutex> ScopedLock;

