#include <stdio.h>
#include "include/os_memory.h"
#include "include/os_logger.h"
#include "include/os_thread.h"
#include "include/smart_ptr.h"

#define LOG_TAG "smart_ptr_test"

struct smart_ptr_test {
    int val;
    const char *str;
};

static void smart_ptr_test_free(void *data)
{
    struct smart_ptr_test *ptr = (struct smart_ptr_test *)data;

    OS_LOGD(LOG_TAG, "Free %p, str=%s", ptr, ptr->str);
    OS_FREE(ptr->str);
}

int main()
{
    struct smart_ptr_test *ptr = NULL;

    ptr = SMART_PTR_NEW(sizeof(struct smart_ptr_test), smart_ptr_test_free);
    OS_LOGD(LOG_TAG, "New %p", ptr);
    SMART_PTR_DUMP();

    ptr->str = OS_STRDUP("hello");

    OS_LOGD(LOG_TAG, "Get %p", ptr);
    SMART_PTR_GET(ptr);
    SMART_PTR_DUMP();

    OS_LOGD(LOG_TAG, "Put %p", ptr);
    SMART_PTR_PUT(ptr);
    SMART_PTR_DUMP();

    OS_LOGD(LOG_TAG, "Put %p", ptr);
    SMART_PTR_PUT(ptr);
    SMART_PTR_DUMP();

    return 0;
}
