#include <stdio.h>
#include <string.h>
#include "cutils/log_helper.h"
#include "cutils/memory_helper.h"

#define LOG_TAG "memdbgtest"

int main()
{
    void *ptr1 = OS_MALLOC(1);
    void *ptr2 = OS_CALLOC(1, 2);
    OS_MEMORY_DUMP();

    ptr2 = OS_REALLOC(ptr2, 3);
    OS_MEMORY_DUMP();

    void *ptr3 = OS_STRDUP("Hello, world");
    OS_FREE(ptr1);
    OS_FREE(ptr2);
    OS_FREE(ptr3);
    OS_MEMORY_DUMP();

    const char *str = "we product overflow here";
    void *ptr4 = OS_MALLOC(strlen(str));
    sprintf(ptr4, "%s", str);
    OS_MEMORY_DUMP();

    //OS_FREE(ptr4);
    return 0;
}
