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

#include "msgutils/os_thread.h"
#include "msgutils/os_time.h"
#include "msgutils/os_logger.h"
#include "msgutils/os_class.hpp"
#include "msgutils/Namespace.hpp"
#include "msgutils/Looper.hpp"

#define TAG "Looper"

#define DEFAULT_LOOPER_PRIORITY  OS_THREAD_PRIO_NORMAL
#define DEFAULT_LOOPER_STACKSIZE 1024

MSGUTILS_NAMESPACE_BEGIN

static const int kCacheMsgMaxCount = 20;
static int kCacheMsgCount = 0;
static Message *kCacheMsgList = NULL;
static Mutex kCacheMsgMutex;

Message *Message::obtain(int what)
{
    return Message::obtain(what, 0, 0, NULL, NULL);
}

Message *Message::obtain(int what, int arg1, int arg2)
{
    return Message::obtain(what, arg1, arg2, NULL, NULL);
}

Message *Message::obtain(int what, int arg1, int arg2, void *data)
{
    return Message::obtain(what, arg1, arg2, data, NULL);
}

Message *Message::obtain(int what, int arg1, int arg2, void *data, HandlerCallback *handlerCallback)
{
    Message *msg = NULL;

    {
        Mutex::Autolock _l(kCacheMsgMutex);
        if (kCacheMsgList) {
            Message *next = kCacheMsgList->next;
            msg = kCacheMsgList;
            kCacheMsgList = next;
            kCacheMsgCount--;
        }
    }

    if (msg == NULL)
        OS_NEW(msg, Message);

    msg->what = what;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->data = data;
    msg->handlerCallback = handlerCallback;
    msg->next = NULL;
    msg->when = 0;
    return msg;
}

Message::Message()
    : handlerCallback(NULL),
      next(NULL)
{}

int Message::count()
{
    int count = 0;
    Message *next = this;
    while (next) {
        count++;
        next = next->next;
    }
    return count;
}

void Message::reset()
{
    if (this->data && this->handlerCallback)
        this->handlerCallback->onFree(this);
    this->what = 0;
    this->arg1 = 0;
    this->arg2 = 0;
    this->data = NULL;
    this->handlerCallback = NULL;
    this->next = NULL;
    this->when = 0;
}

void Message::recycle()
{
    Message *msg = this;
    bool cached = false;

    msg->reset();

    {
        Mutex::Autolock _l(kCacheMsgMutex);
        if (kCacheMsgCount < kCacheMsgMaxCount) {
            if (kCacheMsgList)
                msg->next = kCacheMsgList;
            kCacheMsgList = msg;
            kCacheMsgCount++;
            cached = true;
        }
    }

    if (!cached)
        OS_DELETE(msg);
}

void Message::destroy()
{
    Message *next = this->next, *msg;
    while (next) {
        msg = next;
        next = next->next;
        msg->recycle();
    }
    this->recycle();
}

Looper::Looper(const char *looperName)
    : mMsgList(NULL),
      mExitPending(false),
      mExited(true)
{
    mLooperName = looperName ? looperName : "Looper";
}

Looper::~Looper()
{
    quitSafely();
    if (mMsgList)
        mMsgList->destroy();
}

void Looper::loop()
{
    OS_LOGD(TAG, "[%s]: Entry looper thread", mLooperName.c_str());

    mExited = false;
    while (!mExitPending) {
        Message *msg = NULL;
        unsigned long long now;

        {
            Mutex::Autolock _l(mMsgMutex);

            while (mMsgList == NULL && !mExitPending) {
                OS_LOGV(TAG, "[%s]: mMsgMutex condWait, waiting", mLooperName.c_str());
                mMsgMutex.condWait();
                OS_LOGV(TAG, "[%s]: mMsgMutex condWait, wakeup", mLooperName.c_str());
            }
            if (mExitPending)
                break;

            msg = mMsgList;
            now = OS_MONOTONIC_USEC();
            if (msg->when > now) {
                unsigned long long wait = msg->when - now;
                OS_LOGV(TAG, "[%s]: mMsgMutex(what=%d, when=%llu) condWait(%llu), waiting",
                        mLooperName.c_str(), msg->what, msg->when, wait);
                mMsgMutex.condWait(wait);
                OS_LOGV(TAG, "[%s]: mMsgMutex(what=%d, when=%llu) condWait(%llu), wakeup",
                        mLooperName.c_str(),  msg->what, msg->when, wait);
                msg = NULL;
            }
            else {
                mMsgList = msg->next;
                msg->next = NULL;
            }
        }

        if (msg) {
            if (msg->handlerCallback)
                msg->handlerCallback->onHandle(msg);
            else
                OS_LOGE(TAG, "[%s]: No handler, message what=%d", mLooperName.c_str(), msg->what);
            msg->recycle();
        }
    }

    {
        Mutex::Autolock _l(mExitMutex);
        mExited = true;
        mExitMutex.condSignal();
    }

    OS_LOGD(TAG, "[%s]: Leave looper thread", mLooperName.c_str());
}

void Looper::quit()
{
    Mutex::Autolock _l(mMsgMutex);
    mExitPending = true;
    mMsgMutex.condSignal();
}

void Looper::quitSafely()
{
    {
        Mutex::Autolock _l(mMsgMutex);
        mExitPending = true;
        mMsgMutex.condSignal();
    }

    {
        Mutex::Autolock _l(mExitMutex);
        while (!mExited) {
            OS_LOGV(TAG, "[%s]: mExitMutex condWait, waiting", mLooperName.c_str());
            mExitMutex.condWait();
            OS_LOGV(TAG, "[%s]: mExitMutex condWait, wakeup", mLooperName.c_str());
        }
    }
}

bool Looper::postMessage(Message *msg)
{
    return postMessageDelay(msg, 0);
}

bool Looper::postMessageDelay(Message *msg, unsigned long delayMs)
{
    if (msg == NULL || msg->handlerCallback == NULL)
        return false;

    msg->when = OS_MONOTONIC_USEC() + delayMs * 1000;
    msg->next = NULL;

    {
        Mutex::Autolock _l(mMsgMutex);

        if (mMsgList && mMsgList->when <= msg->when) {
            Message *next = mMsgList->next;
            Message *curr = mMsgList;
            while (next) {
                if (next->when > msg->when) {
                    msg->next = next;
                    break;
                }
                else {
                    curr = next;
                }
                next = next->next;
            }
            curr->next = msg;
        }
        else {
            if (mMsgList)
                msg->next = mMsgList;
            mMsgList = msg;
        }

        mMsgMutex.condSignal();
    }
    return true;
}

bool Looper::postMessageFront(Message *msg)
{
    if (msg == NULL || msg->handlerCallback == NULL)
        return false;

    msg->when = OS_MONOTONIC_USEC();
    msg->next = NULL;

    {
        Mutex::Autolock _l(mMsgMutex);

        if (mMsgList) {
            if (msg->when > mMsgList->when)
                msg->when = mMsgList->when;
            msg->next = mMsgList;
        }
        mMsgList = msg;

        mMsgMutex.condSignal();
    }
    return true;
}

void Looper::removeMessage(int what)
{
    Message *deleteList = NULL;

    {
        Mutex::Autolock _l(mMsgMutex);

        Message *curr = mMsgList, *prev = NULL, *next = NULL;
        while (curr) {
            next = curr->next;
            if (curr->what == what) {
                if (prev)
                    prev->next = curr->next;
                else
                    mMsgList = curr->next;
                curr->next = deleteList;
                deleteList = curr;
            }
            else {
                prev = curr;
            }
            curr = next;
        }
    }

    if (deleteList)
        deleteList->destroy();
}

void Looper::removeMessage()
{
    Mutex::Autolock _l(mMsgMutex);
    if (mMsgList) {
        mMsgList->destroy();
        mMsgList = NULL;
    }
}

bool Looper::hasMessage(int what)
{
    Mutex::Autolock _l(mMsgMutex);
    Message *msg = mMsgList;
    while (msg) {
        if (msg->what == what) {
            return true;
        }
        msg = msg->next;
    }
    return false;
}

bool Looper::hasExited()
{
    return mExited;
}

void Looper::dump()
{
    Mutex::Autolock _l(mMsgMutex);

    OS_LOGI(TAG, "[%s]: Dump looper message:", mLooperName.c_str());
    OS_LOGI(TAG, " > looper_name=[%s]", mLooperName.c_str());
    OS_LOGI(TAG, " > looper_exit=[%s]", mExited ? "true" : "false");
    OS_LOGI(TAG, " > message_count=[%d]", mMsgList ? mMsgList->count() : 0);

    Message *msg = mMsgList;
    int i = 0;
    while (msg) {
        OS_LOGI(TAG, "   > [%d]: what=[%d], arg1=[%d], arg2=[%d], when=[%llu]",
                             i, msg->what, msg->arg1, msg->arg2, msg->when);
        msg = msg->next;
        i++;
    }
}

Handler::Handler(Looper *looper, HandlerCallback *callback)
    : mHandlerCallback(callback),
      mLooper(looper)
{}

Handler::Handler(Looper *looper)
    : mHandlerCallback(NULL),
      mLooper(looper)
{}

Handler::Handler()
    : mHandlerCallback(NULL),
      mLooper(NULL)
{}

void Handler::setHandlerCallback(HandlerCallback *callback)
{
    mHandlerCallback = callback;
}

HandlerCallback *Handler::getHandlerCallback()
{
    return mHandlerCallback;
}

void Handler::setLooper(Looper *looper)
{
    mLooper = looper;
}

Looper *Handler::getLooper()
{
    return mLooper;
}

void Handler::onHandle(Message *msg)
{
    if (mHandlerCallback)
        mHandlerCallback->onHandle(msg);
    else
        OS_LOGE(TAG, "No handler, message what=%d", msg->what);
}

void Handler::onFree(Message *msg)
{
    if (mHandlerCallback)
        mHandlerCallback->onFree(msg);
    else
        OS_LOGE(TAG, "No handler, message what=%d", msg->what);
}

bool Handler::postMessage(Message *msg)
{
    return postMessageDelay(msg, 0);
}

bool Handler::postMessageDelay(Message *msg, unsigned long delayMs)
{
    if (msg) {
        if (msg->handlerCallback == NULL)
            msg->handlerCallback = this;
        if (mLooper && mLooper->postMessageDelay(msg, delayMs)) {
            return true;
        }
        OS_LOGE(TAG, "No looper, message what=%d", msg->what);
        msg->recycle();
    }
    return false;
}

bool Handler::postMessageFront(Message *msg)
{
    if (msg) {
        if (msg->handlerCallback == NULL)
            msg->handlerCallback = this;
        if (mLooper && mLooper->postMessageFront(msg)) {
            return true;
        }
        OS_LOGE(TAG, "No looper, message what=%d", msg->what);
        msg->recycle();
    }
    return false;
}

void Handler::removeMessage(int what)
{
    if (mLooper)
        mLooper->removeMessage(what);
}

void Handler::removeMessage()
{
    if (mLooper)
        mLooper->removeMessage();
}

bool Handler::hasMessage(int what)
{
    if (mLooper)
        return mLooper->hasMessage(what);
    return false;
}

void Handler::dump()
{
    if (mLooper)
        mLooper->dump();
}

HandlerThread::HandlerThread(const char *threadName)
    : mLooper(NULL),
      mThreadPriority(DEFAULT_LOOPER_PRIORITY),
      mThreadStacksize(DEFAULT_LOOPER_STACKSIZE),
      mHasStarted(false)
{
    mThreadName = threadName ? threadName : "HandlerThread";
}

HandlerThread::HandlerThread(const char *threadName, enum os_threadprio threadPriority, unsigned int threadStacksize)
    : mLooper(NULL),
      mThreadPriority(threadPriority),
      mThreadStacksize(threadStacksize),
      mIsRunning(false),
      mHasStarted(false)
{
    mThreadName = threadName ? threadName : "HandlerThread";
}

HandlerThread::~HandlerThread()
{
    OS_DELETE(mLooper);
}

void *HandlerThread::threadEntry(void *arg)
{
    HandlerThread * const thiz = static_cast<HandlerThread *>(arg);
    OS_LOGD(TAG, "[%s]: Entry handler thread", thiz->mThreadName.c_str());
    if (!thiz->mLooper) {
        OS_NEW(thiz->mLooper, Looper, thiz->mThreadName.c_str());
        {
            Mutex::Autolock _l(thiz->mThreadMutex);
            thiz->mIsRunning = true;
            thiz->mThreadMutex.condSignal();
        }
        thiz->mLooper->loop();
    }
    OS_LOGD(TAG, "[%s]: Leave handler thread", thiz->mThreadName.c_str());
    return NULL;
}

bool HandlerThread::start()
{
    Mutex::Autolock _l(mThreadMutex);

    if (!mHasStarted) {
        struct os_threadattr attr = {
            .name = mThreadName.c_str(),
            .priority = mThreadPriority,
            .stacksize = mThreadStacksize,
            .joinable = false,
        };
        os_thread_t tid = OS_THREAD_CREATE(&attr, threadEntry, this);
        if (tid != NULL){
            OS_THREAD_SET_NAME(tid, mThreadName.c_str());
            mHasStarted = true;
        }
    }
    return mHasStarted;
}

void HandlerThread::stop()
{
    if (mLooper)
        mLooper->quit();
}

void HandlerThread::stopSafely()
{
    if (mLooper)
        mLooper->quitSafely();
}

Looper *HandlerThread::getLooper()
{
    if (!this->start())
        return NULL;

    Mutex::Autolock _l(mThreadMutex);
    while (!mIsRunning) {
        OS_LOGV(TAG, "[%s]: mThreadMutex condWait, waiting", mThreadName.c_str());
        mThreadMutex.condWait();
        OS_LOGV(TAG, "[%s]: mThreadMutex condWait, wakeup", mThreadName.c_str());
    }

    return mLooper;
}

const char *HandlerThread::getThreadName()
{
    return mThreadName.c_str();
}

bool HandlerThread::isRunning()
{
    return mIsRunning;
}

MSGUTILS_NAMESPACE_END
