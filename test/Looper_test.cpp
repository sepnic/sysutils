#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "utils/UtilsCommon.h"
#include "utils/Looper.h"

#define LOG_TAG "LooperTest"

SYSUTILS_NAMESPACE_USING

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
    mHandlerThread->run();
}

LooperTest::~LooperTest()
{
    mHandlerThread->requestExitAndWait();
    OS_DELETE(mHandler);
    OS_DELETE(mHandlerThread);
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
    mHandler->postMessage(msg);
}

void LooperTest::postMessageDelay(Message *msg, unsigned long delayMs)
{
    mHandler->postMessageDelay(msg, delayMs);
}

void LooperTest::postMessageFront(Message *msg)
{
    mHandler->postMessageFront(msg);
}

void LooperTest::removeMessage(int what)
{
    mHandler->removeMessage(what);
}

void LooperTest::dump()
{
    mHandler->dump();
}

int main()
{
    LooperTest *looperTest;
    OS_NEW(looperTest, LooperTest);

    Message *msg1 = Message::obtain(2000, OS_STRDUP("postMessageDelay(msg1, 2000)"));
    looperTest->postMessageDelay(msg1, 2000); // delay 2S;

    Message *msg2 = Message::obtain(1000, OS_STRDUP("postMessageDelay(msg2, 1000)"));
    looperTest->postMessageDelay(msg2, 1000); // delay 1S;

    Message *msg3 = Message::obtain(1, OS_STRDUP("postMessage(msg3)"));
    looperTest->postMessage(msg3);

    Message *msg4 = Message::obtain(2, OS_STRDUP("postMessage(msg4)"));
    looperTest->postMessage(msg4);

    Message *msg5 = Message::obtain(0, OS_STRDUP("postMessageFront(msg5)"));
    looperTest->postMessageFront(msg5);

    Message *msg6 = Message::obtain(-1, OS_STRDUP("postMessageDelay(msg6, 1000)"));
    looperTest->postMessageDelay(msg6, 1000);

    Message *msg7 = Message::obtain(-1, OS_STRDUP("postMessageDelay(msg7, 2000)"));
    looperTest->postMessageDelay(msg7, 2000);

    looperTest->removeMessage(-1);
    looperTest->dump();

    //OS_LOGW(LOG_TAG, "-->Dump class after post message");
    //OS_CLASS_DUMP();

    sleep(3);
    OS_DELETE(looperTest);

    //OS_LOGW(LOG_TAG, "-->Dump class after delete looper");
    //OS_CLASS_DUMP();
    return 0;
}
