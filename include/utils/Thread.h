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

#ifndef __MSGUTILS_THREAD_BASE_H__
#define __MSGUTILS_THREAD_BASE_H__

#include <stdio.h>
#include <stdbool.h>
#include <string>
#include "cutils/os_thread.h"
#include "Mutex.h"
#include "Namespace.h"

MSGUTILS_NAMESPACE_BEGIN

class Thread
{
public:
    // Create a Thread object, but doesn't create or start the associated
    // thread. See the run() method.
                        Thread();
    virtual             ~Thread();

    // Start the thread in threadLoop() which needs to be implemented.
    virtual bool        run(    const char *name = 0,
                                enum os_threadprio priority = OS_THREAD_PRIO_NORMAL,
                                unsigned int stack = 1024);

    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void        requestExit();

    // Good place to do one-time initializations
    virtual bool        readyToRun();

    // Call requestExit() and wait until this object's thread exits.
    // BE VERY CAREFUL of deadlocks. In particular, it would be silly to call
    // this function from this object's thread. Will return WOULD_BLOCK in
    // that case.
            bool        requestExitAndWait();

    // Wait until this object's thread exits. Returns immediately if not yet running.
    // Do not call from this object's thread.
            bool        join();

    // Indicates whether this thread is running or not.
            bool        isRunning();

protected:
    // exitPending() returns true if requestExit() has been called.
            bool        exitPending();

private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool        threadLoop() = 0;

private:
    Thread& operator=(const Thread&);
    static  void           *_threadLoop(void *user);
    // always hold mLock when reading or writing
            os_thread_t     mThread;
            Mutex           mLock;
    // note that all accesses of mExitPending and mRunning need to hold mLock
    volatile bool           mExitPending;
    volatile bool           mRunning;
};

MSGUTILS_NAMESPACE_END

#endif /* __MSGUTILS_THREAD_BASE_H__ */
