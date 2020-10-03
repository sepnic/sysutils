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

#ifndef __MSGUTILS_LOOPER_H__
#define __MSGUTILS_LOOPER_H__

#include <stdio.h>
#include <stdbool.h>
#include <string>
#include <list>
#include "cutils/os_thread.h"
#include "Mutex.h"
#include "Namespace.h"

MSGUTILS_NAMESPACE_BEGIN

/**
 * +---------+
 * |         |
 * | Handler +-postMessage()-----+                                   +--------------------------------+
 * |         |                   |                                   | HandlerThread::threadLoop()    |
 * +---------+               +---v-------------------------------+   |  Looper->loop()                |
 * +---------+               | Looper::mMsgList                  |   |                                |
 * |         |               |                                   |   | +----------------------------+ |
 * | Handler +-postMessage()->     +-------+ +-------+ +-------+ +---> | Message::handlerCallback   | |
 * |         |               | ... |Message| |Message| |Message| |   | |  HandlerCallback::onHandler| |
 * +---------+               |     +-------+ +-------+ +-------+ |   | |  HandlerCallback::onFree   | |
 * +---------+               +---^-------------------------------+   | +----------------------------+ |
 * |         |                   |                                   |                                |
 * | Handler +-postMessage()-----+                                   +--------------------------------+
 * |         |
 * +---------+
 */

class Message;
class Looper;
class Handler;

class HandlerCallback {
public:
    virtual void onHandle(Message *msg) = 0;
    virtual void onFree(Message *msg) {} // callback to free message data
protected:
    virtual ~HandlerCallback() {}
};

/**
 * Defines a message containing a description and arbitrary data object that can be
 * sent to a {Handler}. This object contains two extra int fields and an extra
 * object field that allow you to not do allocations in many cases.
 *
 * While the constructor of Message is public, the best way to get one of these is
 * to call {obtain()} methods, which will pull them from a pool of recycled objects.
 */
class Message {
    friend class Looper;
    friend class Handler;

public:
    int   what;
    int   arg1;
    int   arg2;
    void *data;

    static Message *obtain(int what);
    static Message *obtain(int what, void *data);
    static Message *obtain(int what, int arg1, int arg2);
    static Message *obtain(int what, int arg1, int arg2, void *data);

private:
    HandlerCallback *handlerCallback;
    unsigned long long when;

    Message();
    void reset();
    void recycle();
};

/**
 * Class used to run a message loop for a thread. Threads by default do
 * not have a message loop associated with them; to create one, call
 * {Looper()} in the thread that is to run the loop, and then {loop()} to
 * have it process messages until the loop is stopped.
 *
 * Most interaction with a message loop is through the {Handler} class.
 */
class Looper {
public:
    Looper(const char *name = 0);
    ~Looper();

    void loop();
    void quit();
    void quitSafely();
    void waitRunning();
    bool isRunning();

    bool postMessage(Message *msg);
    bool postMessageDelay(Message *msg, unsigned long delayMs);
    bool postMessageFront(Message *msg);
    void removeMessage(int what, HandlerCallback *handlerCallback);
    void removeMessage(HandlerCallback *handlerCallback);
    bool hasMessage(int what, HandlerCallback *handlerCallback);
    void dump();

private:
    std::string mLooperName;
    std::list<Message *> mMsgList;
    Mutex mMsgMutex;
    Mutex mStateMutex;
    bool mExitPending;
    bool mRunning;
};

/**
 * A Handler allows you to send and process {Message} and Runnable objects
 * associated with a thread's {Looper}.  Each Handler instance is associated
 * with a single thread and that thread's looper.  When you create a new
 * Handler, it is bound to the thread / looper of the thread that  is creating
 * it -- from that point on, it will deliver messages and runnables to that
 * message queue and execute them as they come out of the message queue.
 *
 * There are two main uses for a Handler: (1) to schedule messages and
 * runnables to be executed as some point in the future; and (2) to enqueue
 * an action to be performed on a different thread than your own.
 *
 * When posting or sending to a Handler, you can either allow the item to be
 * processed as soon as the message queue is ready to do so, or specify a
 * delay before it gets processed or absolute time for it to be processed.
 * The latter two allow you to implement timeouts,ticks, and other
 * timing-based behavior.
 *
 * When a process is created for your application, its main thread is
 * dedicated to running a message queue that takes care of managing the
 * top-level application objects (activities, broadcast receivers, etc) and
 * any windows they create.  You can create your own threads, and communicate
 * back with the main application thread through a Handler.  This is done by
 * calling the same {postMessage()} methods as before, but from your new
 * thread.  The given Runnable or Message will then be scheduled in the
 * Handler's message queue and processed when appropriate.
 */
class Handler : public HandlerCallback {
public:
    Handler(Looper *looper, HandlerCallback *callback);
    Handler(Looper *looper);
    Handler();
    ~Handler(); // Must do destructor before main HandlerThread exit

    void setHandlerCallback(HandlerCallback *callback);
    HandlerCallback *getHandlerCallback();
    void setLooper(Looper *looper);
    Looper *getLooper();

    virtual void onHandle(Message *msg);
    virtual void onFree(Message *msg);

    bool postMessage(Message *msg);
    bool postMessageDelay(Message *msg, unsigned long delayMs);
    bool postMessageFront(Message *msg);
    void removeMessage(int what);
    void removeMessage();
    bool hasMessage(int what);
    void dump();

private:
    HandlerCallback *mHandlerCallback;
    Looper *mLooper;
};

/**
 * HandlerThread class for starting a new thread that has a looper. The looper can
 * then be used to create handler classes. Note that run() must still be called.
 */
class HandlerThread {
public:
    HandlerThread(const char *name = 0,
                  enum os_threadprio priority = OS_THREAD_PRIO_NORMAL,
                  unsigned int stacksize = 1024);
    ~HandlerThread();

    bool run();
    void requestExit();
    void requestExitAndWait();
    bool isRunning();
    Looper *getLooper();

private:
    Looper *mLooper;
    os_thread_t mThreadId;
    std::string mThreadName;
    enum os_threadprio mThreadPriority;
    unsigned int mThreadStacksize;
    Mutex mThreadMutex;
    bool mRunning;
    static void *threadEntry(void *arg);
};

MSGUTILS_NAMESPACE_END

#endif /* __MSGUTILS_LOOPER_H__ */
