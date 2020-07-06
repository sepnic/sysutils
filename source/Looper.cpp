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

MSGUTILS_NAMESPACE_BEGIN

static const int kCacheMsgMaxCount = 50;
std::list<Message *> kCacheMsgList;
static Mutex kCacheMsgMutex;

Message *Message::obtain(int what)
{
    return Message::obtain(what, 0, 0, NULL);
}

Message *Message::obtain(int what, void *data)
{
    return Message::obtain(what, 0, 0, data);
}

Message *Message::obtain(int what, int arg1, int arg2)
{
    return Message::obtain(what, arg1, arg2, NULL);
}

Message *Message::obtain(int what, int arg1, int arg2, void *data)
{
    Message *msg = NULL;

    {
        Mutex::Autolock _l(kCacheMsgMutex);
        if (!kCacheMsgList.empty()) {
            msg = kCacheMsgList.front();
            kCacheMsgList.pop_front();
        }
    }

    if (msg == NULL)
        OS_NEW(msg, Message);

    msg->what = what;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->data = data;
    msg->handlerCallback = NULL;
    msg->when = 0;
    return msg;
}

Message::Message()
    : handlerCallback(NULL)
{}

void Message::reset()
{
    if (this->data && this->handlerCallback)
        this->handlerCallback->onFree(this);
    this->what = 0;
    this->arg1 = 0;
    this->arg2 = 0;
    this->data = NULL;
    this->handlerCallback = NULL;
    this->when = 0;
}

void Message::recycle()
{
    Message *msg = this;
    msg->reset();

    {
        Mutex::Autolock _l(kCacheMsgMutex);
        if (kCacheMsgList.size() < kCacheMsgMaxCount) {
            kCacheMsgList.push_back(msg);
            msg = NULL;
        }
    }

    if (msg)
        OS_DELETE(msg);
}

Looper::Looper(const char *name)
    : mLooperName(name ? name : "Looper"),
      mExitPending(false),
      mRunning(false)
{
    mMsgList.clear();
}

Looper::~Looper()
{
    if (mRunning)
        quitSafely();

    std::list<Message *>::iterator it;
    for (it = mMsgList.begin(); it != mMsgList.end(); it++)
        (*it)->recycle();
}

void Looper::loop()
{
    OS_LOGD(TAG, "[%s]: Entry looper thread", mLooperName.c_str());

    {
        Mutex::Autolock _l(mStateMutex);
        mExitPending = false;
        mRunning = true;
        mStateMutex.condBroadcast();
    }

    while (1) {
        Message *msg = NULL;
        {
            Mutex::Autolock _l(mMsgMutex);

            while (mMsgList.empty() && !mExitPending) {
                OS_LOGV(TAG, "[%s]: mMsgMutex condWait, waiting", mLooperName.c_str());
                mMsgMutex.condWait();
                OS_LOGV(TAG, "[%s]: mMsgMutex condWait, wakeup", mLooperName.c_str());
            }
            if (mExitPending)
                break;

            msg = mMsgList.front();
            unsigned long long now = OS_MONOTONIC_USEC();
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
                mMsgList.pop_front();
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
        Mutex::Autolock _l(mStateMutex);
        mRunning = false;
        mStateMutex.condBroadcast();
    }

    OS_LOGD(TAG, "[%s]: Leave looper thread", mLooperName.c_str());
}

void Looper::quit()
{
    Mutex::Autolock _l(mStateMutex);
    {
        Mutex::Autolock _l(mMsgMutex);
        mExitPending = true;
        mMsgMutex.condSignal();
    }
}

void Looper::quitSafely()
{
    Mutex::Autolock _l(mStateMutex);
    {
        Mutex::Autolock _l(mMsgMutex);
        mExitPending = true;
        mMsgMutex.condSignal();
    }
    while (mRunning == true) {
        OS_LOGV(TAG, "[%s]: mStateMutex condWait, waiting", mLooperName.c_str());
        mStateMutex.condWait();
        OS_LOGV(TAG, "[%s]: mStateMutex condWait, wakeup", mLooperName.c_str());
    }
}

void Looper::waitRunning()
{
    Mutex::Autolock _l(mStateMutex);
    while (mRunning == false) {
        OS_LOGV(TAG, "[%s]: mStateMutex condWait, waiting", mLooperName.c_str());
        mStateMutex.condWait();
        OS_LOGV(TAG, "[%s]: mStateMutex condWait, wakeup", mLooperName.c_str());
    }
}

bool Looper::isRunning()
{
    Mutex::Autolock _l(mStateMutex);
    return mRunning;
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
    {
        Mutex::Autolock _l(mMsgMutex);

        std::list<Message *>::reverse_iterator rit;
        for (rit = mMsgList.rbegin(); rit != mMsgList.rend(); rit++) {
            if (msg->when >= (*rit)->when) {
                // rit.base() pointing to the element that followed the element referred to rit
                mMsgList.insert(rit.base(), msg);
                break;
            }
        }
        if (rit == mMsgList.rend())
            mMsgList.push_front(msg);

        mMsgMutex.condSignal();
    }
    return true;
}

bool Looper::postMessageFront(Message *msg)
{
    if (msg == NULL || msg->handlerCallback == NULL)
        return false;

    msg->when = OS_MONOTONIC_USEC();
    {
        Mutex::Autolock _l(mMsgMutex);

        if (!mMsgList.empty()) {
            Message *front = mMsgList.front();
            if (msg->when > front->when)
                msg->when = front->when;
        }
        mMsgList.push_front(msg);

        mMsgMutex.condSignal();
    }
    return true;
}

void Looper::removeMessage(int what, HandlerCallback *handlerCallback)
{
    Mutex::Autolock _l(mMsgMutex);
    std::list<Message *>::iterator it;
    for (it = mMsgList.begin(); it != mMsgList.end(); ) {
        if ((*it)->what == what && (*it)->handlerCallback == handlerCallback) {
            (*it)->recycle();
            it = mMsgList.erase(it);
        }
        else {
            it++;
        }
    }
}

void Looper::removeMessage(HandlerCallback *handlerCallback)
{
    Mutex::Autolock _l(mMsgMutex);
    std::list<Message *>::iterator it;
    for (it = mMsgList.begin(); it != mMsgList.end(); ) {
        if ((*it)->handlerCallback == handlerCallback) {
            (*it)->recycle();
            it = mMsgList.erase(it);
        }
        else {
            it++;
        }
    }
}

bool Looper::hasMessage(int what, HandlerCallback *handlerCallback)
{
    Mutex::Autolock _l(mMsgMutex);
    std::list<Message *>::iterator it;
    for (it = mMsgList.begin(); it != mMsgList.end(); it++) {
        if ((*it)->what == what && (*it)->handlerCallback == handlerCallback)
            return true;
    }
    return false;
}

void Looper::dump()
{
    Mutex::Autolock _l(mMsgMutex);

    OS_LOGI(TAG, "[%s]: Dump looper messages:", mLooperName.c_str());
    OS_LOGI(TAG, " > Name     : %s", mLooperName.c_str());
    OS_LOGI(TAG, " > Running  : %s", mRunning ? "true" : "false");
    OS_LOGI(TAG, " > Messages : %d", (int)mMsgList.size());

    std::list<Message *>::iterator it;
    int i = 0;
    for (it = mMsgList.begin(); it != mMsgList.end(); it++, i++) {
        OS_LOGI(TAG, "   > [%d]: handler=[%p], what=[%d], arg1=[%d], arg2=[%d], when=[%llu]",
                i, (*it)->handlerCallback, (*it)->what, (*it)->arg1, (*it)->arg2, (*it)->when);
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

Handler::~Handler()
{
    removeMessage();
}

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
        msg->handlerCallback = this;
        if (mLooper && mLooper->postMessageDelay(msg, delayMs)) {
            return true;
        }
        OS_LOGE(TAG, "No looper, discard message what=%d", msg->what);
        msg->recycle();
    }
    return false;
}

bool Handler::postMessageFront(Message *msg)
{
    if (msg) {
        msg->handlerCallback = this;
        if (mLooper && mLooper->postMessageFront(msg)) {
            return true;
        }
        OS_LOGE(TAG, "No looper, discard message what=%d", msg->what);
        msg->recycle();
    }
    return false;
}

void Handler::removeMessage(int what)
{
    if (mLooper)
        mLooper->removeMessage(what, this);
}

void Handler::removeMessage()
{
    if (mLooper)
        mLooper->removeMessage(this);
}

bool Handler::hasMessage(int what)
{
    if (mLooper)
        return mLooper->hasMessage(what, this);
    return false;
}

void Handler::dump()
{
    if (mLooper)
        mLooper->dump();
}

HandlerThread::HandlerThread(const char *name, enum os_threadprio priority, unsigned int stacksize)
    : mThreadId(NULL),
      mThreadName(name ? name : "HandlerThread"),
      mThreadPriority(priority),
      mThreadStacksize(stacksize),
      mRunning(false)
{
    OS_NEW(mLooper, Looper, mThreadName.c_str());
}

HandlerThread::~HandlerThread()
{
    if (mRunning)
        requestExitAndWait();
    OS_DELETE(mLooper);
}

void *HandlerThread::threadEntry(void *arg)
{
    HandlerThread * const thiz = static_cast<HandlerThread *>(arg);
    thiz->mLooper->loop();
    {
        Mutex::Autolock _l(thiz->mThreadMutex);
        thiz->mThreadId = NULL;
        thiz->mRunning = false;
        thiz->mThreadMutex.condSignal();
    }
    return NULL;
}

bool HandlerThread::run()
{
    Mutex::Autolock _l(mThreadMutex);

    if (mRunning) {
        OS_LOGD(TAG, "[%s]: HandlerThread already run", mThreadName.c_str());
        return true;
    }

    struct os_threadattr attr = {
        .name = mThreadName.c_str(),
        .priority = mThreadPriority,
        .stacksize = mThreadStacksize,
        .joinable = false,
    };
    mThreadId = OS_THREAD_CREATE(&attr, threadEntry, this);
    if (mThreadId != NULL){
        OS_THREAD_SET_NAME(mThreadId, mThreadName.c_str());
        // waiting for looper enter running (mLooper->isRunning() == true)
        mLooper->waitRunning();
        OS_LOGD(TAG, "[%s]: Looper is running", mThreadName.c_str());
        mRunning = true;
    }
    return mRunning;
}

void HandlerThread::requestExit()
{
    Mutex::Autolock _l(mThreadMutex);
    mLooper->quit();
}

void HandlerThread::requestExitAndWait()
{
    Mutex::Autolock _l(mThreadMutex);

    if (mThreadId == OS_THREAD_SELF()) {
        OS_LOGW(TAG,
                "Thread (%p:%s): don't call requestExitAndWait() from this "
                "Thread object's thread. Maybe deadlock!",
                mThreadId, mThreadName.c_str());
    }

    mLooper->quitSafely();

    while (mRunning == true) {
        OS_LOGV(TAG, "[%s]: mThreadMutex condWait, waiting", mThreadName.c_str());
        mThreadMutex.condWait();
        OS_LOGV(TAG, "[%s]: mThreadMutex condWait, wakeup", mThreadName.c_str());
    }
}

bool HandlerThread::isRunning()
{
    Mutex::Autolock _l(mThreadMutex);
    return mRunning;
}

Looper *HandlerThread::getLooper()
{
    return mLooper;
}

MSGUTILS_NAMESPACE_END
