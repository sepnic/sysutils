/*
 * Copyright (c) 2018-2021 Qinglong<sysu.zqlong@gmail.com>
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

#ifndef __SYSUTILS_MUTEX_H__
#define __SYSUTILS_MUTEX_H__

#include "osal/os_thread.h"
#include "UtilsCommon.h"

SYSUTILS_NAMESPACE_BEGIN

class Mutex {
public:
    Mutex() {
        mMutex = os_mutex_create();
        mCond  = os_cond_create();
    }

    ~Mutex() {
        os_mutex_destroy(mMutex);
        os_cond_destroy(mCond);
    }

    void lock() { os_mutex_lock(mMutex); }
    bool tryLock() { return (0 == os_mutex_trylock(mMutex)); }
    void unlock() { os_mutex_unlock(mMutex); }

    void condWait() { os_cond_wait(mCond, mMutex); }
    bool condWait(unsigned long usec) { return (0 == os_cond_timedwait(mCond, mMutex, usec)); }
    void condSignal() { os_cond_signal(mCond); }
    void condBroadcast() { os_cond_broadcast(mCond); }

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
    os_mutex mMutex;
    os_cond  mCond;
};

SYSUTILS_NAMESPACE_END

#endif /* __SYSUTILS_MUTEX_H__ */
