#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "include/os_memory.h"
#include "include/os_logger.h"
#include "include/looper.h"

#define LOG_TAG "looper_test"

#define array_size(arr) sizeof(arr)/sizeof((arr)[0])

struct private_data {
    int val;
    const char *str;
};

static void msg_handle(struct message *arg)
{
    struct message *msg = arg;
    struct private_data *private = msg->data;
    OS_LOGD(LOG_TAG, "Handle message: what=[%d], val=[%d], str=[%s]", msg->what, private->val, private->str);
}

void msg_sleep_handle(struct message *arg)
{
    struct message *msg = arg;
    OS_LOGD(LOG_TAG, "Handle message: what=[%d], sleep two seconds that would cause watdog timeout", msg->what);
    sleep(2);
}

static void msg_free(struct message *msg)
{
    struct private_data *private = msg->data;
    OS_LOGD(LOG_TAG, "Free message: what=[%d], val=[%d], str=[%s]", msg->what, private->val, private->str);
    OS_FREE(private->str);
    OS_FREE(msg->data);
    //OS_FREE(msg);
}

/*static void msg_watchdog(void *data)
{
    struct message *msg = data;
    LOG_FATAL(LOG_TAG, "Message what=[%d] bited by watchdog, exit...", msg->what);
    exit(-1);
}*/

static void msg_notify(struct message *msg, enum message_status status)
{
    if (status == MESSAGE_TIMEOUT)
        OS_LOGE(LOG_TAG, "Message what=[%d] timeout", msg->what);
}

int main()
{
    struct os_threadattr attr;
    looper_t looper;
    struct message *msg;

    attr.name = "looper_test_thread";
    attr.priority = OS_THREAD_PRIO_NORMAL;
    attr.stacksize = 1024;
    looper = looper_create(&attr, msg_handle, msg_free);

    looper_dump(looper);

    looper_start(looper);

    {
        struct private_data *private = OS_MALLOC(sizeof(struct  private_data));
        private->val = 1000;
        private->str = OS_STRDUP("looper_post_message_delay 3S");
        // timeout 3 seconds
        msg = message_obtain2(1000, 0, 0, private, 3000, NULL, NULL, msg_notify);
        //msg->timeout_ms = 3001;
        //msg->timeout_fn = msg_notify;
        looper_post_message_delay(looper, msg, 3000); // delay 3 seconds
    }

    {
        struct private_data *private = OS_MALLOC(sizeof(struct  private_data));
        private->val = 2000;
        private->str = OS_STRDUP("looper_post_message_delay 5S");
        msg = message_obtain(2000, 0, 0, private);
        looper_post_message_delay(looper, msg, 5000); // delay 3 seconds
    }

    {
        struct private_data *private;
        int i;
        for (i = 1; i < 10; i++) {
            private = OS_MALLOC(sizeof(struct  private_data));
            private->val = i;
            private->str = OS_STRDUP("looper_post_message");
            msg = message_obtain(i, 0, 0, private);
            looper_post_message(looper, msg);
        }
    }

    {
        struct private_data *private = OS_MALLOC(sizeof(struct  private_data));
        private->val = 0;
        private->str = OS_STRDUP("looper_post_message_front");
        msg = message_obtain(0, 0, 0, private);
        looper_post_message_front(looper, msg);
    }

    {
        struct private_data *private = OS_MALLOC(sizeof(struct  private_data));
        private->val = -1;
        private->str = OS_STRDUP("looper_enable_watchdog, looper_post_message");
        msg = message_obtain2(-1, 0, 0, private, 0, msg_sleep_handle, NULL, NULL);
        looper_post_message(looper, msg);
        //looper_enable_watchdog(looper, 1000, NULL, msg); // watchdog 1000ms timeout
    }

    looper_dump(looper);

    //OS_LOGE(LOG_TAG, "remove what=1000");
    //looper_remove_message(looper, 1000);
    //looper_dump(looper);

    //looper_stop(looper);
    //looper_dump(looper);

    sleep(10);
    looper_destroy(looper);

    //while (true)
    //    sleep(5);
    return 0;
}
