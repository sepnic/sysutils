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

#ifndef __MSGUTILS_LOOPER_H__
#define __MSGUTILS_LOOPER_H__

#include <stdio.h>
#include <stdbool.h>
#include <string>
#include "Mutex.hpp"
#include "namespace_def.hpp"

MSGUTILS_NAMESPACE_BEGIN

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

class Message {
    friend class Looper;
    friend class Handler;

public:
    int what;
    int arg1;
    int arg2;
    void *data;

    static Message *obtain(int what);
    static Message *obtain(int what, int arg1, int arg2);
    static Message *obtain(int what, int arg1, int arg2, void *data);
    static Message *obtain(int what, int arg1, int arg2, void *data, HandlerCallback *handlerCallback);

private:
    HandlerCallback *handlerCallback;
    unsigned long long when;
    Message *next;

    Message();
    int  count();
    void reset();
    void recycle();
    void destroy();
};

class Looper {
public:
    Looper(const char *looperName);
    ~Looper();

    void loop();
    void quit();
    void quitSafely();

    bool postMessage(Message *msg);
    bool postMessageDelay(Message *msg, unsigned long delayMs);
    bool postMessageFront(Message *msg);
    void removeMessage(int what);
    void removeMessage();
    bool hasMessage(int what);
    bool hasExited();
    void dump();

private:
    std::string mLooperName;
    Message *mMsgList;
    Mutex mMsgMutex;
    Mutex mExitMutex;
    bool mExitPending;
    bool mExited;
};

class Handler : public HandlerCallback {
public:
    Handler(Looper *looper, HandlerCallback *callback);
    Handler(Looper *looper);
    Handler();

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

class HandlerThread {
    friend void *handlerThreadEntry(void *arg);

public:
    HandlerThread(const char *threadName);
    HandlerThread(const char *threadName, enum os_threadprio threadPriority, unsigned int threadStacksize);
    ~HandlerThread();

    bool start();
    void stop();
    void stopSafely();

    Looper *getLooper();
    const char *getThreadName();
    bool isRunning();

private:
    Looper *mLooper;
    std::string mThreadName;
    enum os_threadprio mThreadPriority;
    unsigned int mThreadStacksize;
    Mutex mThreadMutex;
    bool mIsRunning;
    bool mHasStarted;
};

MSGUTILS_NAMESPACE_END

#endif /* __MSGUTILS_LOOPER_H__ */
