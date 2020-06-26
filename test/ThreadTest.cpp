#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "msgutils/os_memory.h"
#include "msgutils/os_logger.h"
#include "msgutils/os_thread.h"
#include "msgutils/os_class.hpp"
#include "msgutils/Namespace.hpp"
#include "msgutils/Thread.hpp"

#define LOG_TAG "ThreadTest"

MSGUTILS_NAMESPACE_USING

class ThreadTest : public Thread {
public:
    ThreadTest() {}
    ~ThreadTest() {}

    virtual bool readyToRun();
    virtual bool threadLoop();
};

bool ThreadTest::readyToRun()
{
    OS_LOGI(LOG_TAG, "-->readyToRun");
    return true;
}

bool ThreadTest::threadLoop()
{
    static int count = 0;
    OS_LOGI(LOG_TAG, "-->threadLoop, count=%d", count);
    count++;
    sleep(1);
    return true;
}

int main()
{
    ThreadTest *threadTest;
    OS_NEW(threadTest, ThreadTest);

    threadTest->run();
    sleep(5);

    threadTest->requestExitAndWait();

    OS_DELETE(threadTest);
    return 0;
}
