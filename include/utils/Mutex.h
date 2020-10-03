/*
 * Copyright (C) 2018-2020 luoyun <sysu.zqlong@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MSGUTILS_MUTEX_H__
#define __MSGUTILS_MUTEX_H__

#include <stdbool.h>
#include "cutils/os_thread.h"
#include "Namespace.h"

MSGUTILS_NAMESPACE_BEGIN

class Mutex {
public:
    Mutex() {
        mMutex = OS_THREAD_MUTEX_CREATE();
        mCond  = OS_THREAD_COND_CREATE();
    }

    ~Mutex() {
        OS_THREAD_MUTEX_DESTROY(mMutex);
        OS_THREAD_COND_DESTROY(mCond);
    }

    void lock() { OS_THREAD_MUTEX_LOCK(mMutex); }
    bool tryLock() { return (0 == OS_THREAD_MUTEX_TRYLOCK(mMutex)); }
    void unlock() { OS_THREAD_MUTEX_UNLOCK(mMutex); }

    void condWait() { OS_THREAD_COND_WAIT(mCond, mMutex); }
    bool condWait(unsigned long usec) { return (0 == OS_THREAD_COND_TIMEDWAIT(mCond, mMutex, usec)); }
    void condSignal() { OS_THREAD_COND_SIGNAL(mCond); }
    void condBroadcast() { OS_THREAD_COND_BROADCAST(mCond); }

    // Manages the mutex automatically. It'll be locked when Autolock is
    // constructed and released when Autolock goes out of scope.
    class Autolock {
    public:
        inline explicit Autolock(Mutex& mutex) : mLock(mutex)  { mLock.lock(); }
        inline explicit Autolock(Mutex* mutex) : mLock(*mutex) { mLock.lock(); }
        inline ~Autolock() { mLock.unlock(); }
    private:
        Mutex& mLock;
    };

private:
    os_mutex_t mMutex;
    os_cond_t  mCond;
};

MSGUTILS_NAMESPACE_END

#endif /* __MSGUTILS_MUTEX_H__ */
