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

#include "cutils/os_thread.h"
#include "cutils/os_logger.h"
#include "utils/Namespace.h"
#include "utils/Thread.h"

#define TAG "ThreadBase"

SYSUTILS_NAMESPACE_BEGIN

Thread::Thread()
    :   mThread(NULL),
        mExitPending(false),
        mRunning(false)
{
}

Thread::~Thread()
{
}

bool Thread::readyToRun()
{
    return true;
}

bool Thread::run(const char *name, enum os_threadprio priority, unsigned int stack)
{
    Mutex::Autolock _l(mLock);

    if (mRunning) {
        OS_LOGD(TAG, "Thread already run");
        return true;
    }

    // reset status and exitPending to their default value, so we can
    // try again after an error happened (either below, or in readyToRun())
    mExitPending = false;
    mThread = NULL;
    mRunning = true;

    struct os_threadattr attr = {
        .name = name != NULL ? name : "ThreadBase",
        .priority = priority,
        .stacksize = stack,
        .joinable = false,
    };
    mThread = OS_THREAD_CREATE(&attr, _threadLoop, this);
    if (mThread != NULL) {
        OS_THREAD_SET_NAME(mThread, name);
        mRunning = true;
    }

    return mRunning;
}

void *Thread::_threadLoop(void *user)
{
    Thread * const self = static_cast<Thread *>(user);
    bool first = true;

    while (1) {
        bool ret;
        if (first) {
            first = false;
            ret = self->readyToRun();
            if (ret && !self->exitPending()) {
                // Binder threads (and maybe others) rely on threadLoop
                // running at least once after a successful ::readyToRun()
                // (unless, of course, the thread has already been asked to exit
                // at that point).
                // This is because threads are essentially used like this:
                //   (new ThreadSubclass())->run();
                // The caller therefore does not retain a strong reference to
                // the thread and the thread would simply disappear after the
                // successful ::readyToRun() call instead of entering the
                // threadLoop at least once.
                ret = self->threadLoop();
            }
        }
        else {
            ret = self->threadLoop();
        }

        // establish a scope for mLock
        {
            Mutex::Autolock _l(self->mLock);
            if (ret == false || self->mExitPending) {
                self->mExitPending = true;
                self->mRunning = false;
                // clear thread ID so that requestExitAndWait() does not exit if
                // called by a new thread using the same thread ID as this one.
                self->mThread = NULL;
                // note that interested observers blocked in requestExitAndWait are
                // awoken by broadcast, but blocked on mLock until break exits scope
                self->mLock.condSignal();
                break;
            }
        }
    }

    return NULL;
}

void Thread::requestExit()
{
    Mutex::Autolock _l(mLock);
    mExitPending = true;
}

bool Thread::requestExitAndWait()
{
    Mutex::Autolock _l(mLock);
    if (mThread == OS_THREAD_SELF()) {
        OS_LOGW(TAG,
                "Thread (this=%p): don't call waitForExit() from this "
                "Thread object's thread. It's a guaranteed deadlock!",
                this);

        return false;
    }

    mExitPending = true;
    while (mRunning == true) {
        mLock.condWait();
    }
    return true;
}

bool Thread::join()
{
    Mutex::Autolock _l(mLock);
    if (mThread == OS_THREAD_SELF()) {
        OS_LOGW(TAG,
                "Thread (this=%p): don't call join() from this "
                "Thread object's thread. It's a guaranteed deadlock!",
                this);
        return false;
    }

    while (mRunning == true) {
        mLock.condWait();
    }
    return true;
}

bool Thread::isRunning()
{
    Mutex::Autolock _l(mLock);
    return mRunning;
}

bool Thread::exitPending()
{
    Mutex::Autolock _l(mLock);
    return mExitPending;
}

SYSUTILS_NAMESPACE_END
