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

#include "msgutils/plus/Mutex.h"

MSGUTILS_NAMESPACE_BEGIN

Mutex::Mutex()
{
    mMutex = OS_THREAD_MUTEX_CREATE();
    mCond  = OS_THREAD_COND_CREATE();
}

Mutex::~Mutex()
{
    if (mMutex) OS_THREAD_MUTEX_DESTROY(mMutex);
    if (mCond)  OS_THREAD_COND_DESTROY(mCond);
}

void Mutex::lock()
{
    if (mMutex)
        OS_THREAD_MUTEX_LOCK(mMutex);
}

bool Mutex::tryLock()
{
    bool ret = false;
    if (mMutex)
        ret = (0 == OS_THREAD_MUTEX_TRYLOCK(mMutex));
    return ret;
}

void Mutex::unlock()
{
    if (mMutex)
        OS_THREAD_MUTEX_UNLOCK(mMutex);
}

void Mutex::condWait()
{
    if (mMutex && mCond)
        OS_THREAD_COND_WAIT(mCond, mMutex);
}

bool Mutex::condWait(unsigned long usec)
{
    bool ret = false;
    if (mMutex && mCond)
        ret = (0 == OS_THREAD_COND_TIMEDWAIT(mCond, mMutex, usec));
    return ret;
}

void Mutex::condSignal()
{
    if (mCond)
        OS_THREAD_COND_SIGNAL(mCond);
}

MSGUTILS_NAMESPACE_END
