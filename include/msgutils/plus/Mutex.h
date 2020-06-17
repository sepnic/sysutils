/* The MIT License (MIT)
 *
 * Copyright (c) 2018-2020 luoyun <sysu.zqlong@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __MSGUTILS_MUTEX_H__
#define __MSGUTILS_MUTEX_H__

#include <stdbool.h>
#include "msgutils/os_thread.h"
#include "namespace_def.h"

MSGUTILS_NAMESPACE_BEGIN

class Mutex {
public:
    Mutex();
    ~Mutex();

    void lock();
    bool tryLock();
    void unlock();

    void condWait();
    bool condWait(unsigned long usec);
    void condSignal();

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
