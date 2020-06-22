#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "msgutils/os_memory.h"
#include "msgutils/os_logger.h"
#include "msgutils/os_thread.h"
#include "msgutils/os_class.hpp"
#include "msgutils/namespace_def.hpp"
#include "msgutils/Looper.hpp"

#define LOG_TAG "LooperTest"

MSGUTILS_NAMESPACE_USING

class LooperTest : public HandlerCallback {
public:
    LooperTest();
    ~LooperTest();

    virtual void onHandle(Message *msg);
    virtual void onFree(Message *msg);

    void postMessage(Message *msg);
    void postMessageDelay(Message *msg, unsigned long delayMs);
    void postMessageFront(Message *msg);
    void removeMessage(int what);

    void dump();

private:
    HandlerThread *mHandlerThread;
    Handler *mHandler;
};

LooperTest::LooperTest()
{
    OS_NEW(mHandlerThread, HandlerThread, "LooperTest");
    OS_NEW(mHandler, Handler, mHandlerThread->getLooper(), this);
}

LooperTest::~LooperTest()
{
    if (mHandlerThread)
        mHandlerThread->stopSafely();
    OS_DELETE(mHandlerThread);
    OS_DELETE(mHandler);
}

void LooperTest::onHandle(Message *msg)
{
    OS_LOGI(LOG_TAG, "Handle message: what=%d, str=%s", msg->what, msg->data);
}

void LooperTest::onFree(Message *msg)
{
    OS_LOGI(LOG_TAG, "Free message: what=%d, str=%s", msg->what, msg->data);
    OS_FREE(msg->data);
}

void LooperTest::postMessage(Message *msg)
{
    if (mHandler)
        mHandler->postMessage(msg);
}

void LooperTest::postMessageDelay(Message *msg, unsigned long delayMs)
{
    if (mHandler)
        mHandler->postMessageDelay(msg, delayMs);
}

void LooperTest::postMessageFront(Message *msg)
{
    if (mHandler)
        mHandler->postMessageFront(msg);
}

void LooperTest::removeMessage(int what)
{
    if (mHandler)
        mHandler->removeMessage(what);
}

void LooperTest::dump()
{
    if (mHandler)
        mHandler->dump();
}

int main()
{
    LooperTest *looperTest;
    OS_NEW(looperTest, LooperTest);

    Message *msg1 = Message::obtain(2000, 0, 0, OS_STRDUP("postMessageDelay(msg1, 2000)"), NULL);
    looperTest->postMessageDelay(msg1, 2000); // delay 2S;

    Message *msg2 = Message::obtain(1000, 0, 0, OS_STRDUP("postMessageDelay(msg2, 1000)"), NULL);
    looperTest->postMessageDelay(msg2, 1000); // delay 1S;

    Message *msg3 = Message::obtain(1, 0, 0, OS_STRDUP("postMessage(msg3)"), NULL);
    looperTest->postMessage(msg3);

    Message *msg4 = Message::obtain(2, 0, 0, OS_STRDUP("postMessage(msg4)"), NULL);
    looperTest->postMessage(msg4);

    Message *msg5 = Message::obtain(0, 0, 0, OS_STRDUP("postMessageFront(msg5)"), NULL);
    looperTest->postMessageFront(msg5);

    Message *msg6 = Message::obtain(-1, 0, 0, OS_STRDUP("postMessageDelay(msg6, 1000)"), NULL);
    looperTest->postMessageDelay(msg6, 1000);

    Message *msg7 = Message::obtain(-1, 0, 0, OS_STRDUP("postMessageDelay(msg7, 2000)"), NULL);
    looperTest->postMessageDelay(msg7, 2000);

    looperTest->removeMessage(-1);
    looperTest->dump();

    OS_LOGW(LOG_TAG, "-->Dump memory after post message");
    OS_MEMORY_DUMP();

    sleep(3);
    OS_DELETE(looperTest);

    OS_LOGW(LOG_TAG, "-->Dump memory after delete looper");
    OS_MEMORY_DUMP();
    return 0;
}
