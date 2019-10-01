#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "msglooper/os_memory.h"
#include "msglooper/os_logger.h"
#include "msglooper/os_thread.h"
#include "msglooper/msglooper.h"

#define LOG_TAG "looper_test"

struct priv_data {
    const char *str;
};

static void msg_handle(struct message *arg)
{
    struct message *msg = arg;
    struct priv_data *priv = msg->data;
    OS_LOGD(LOG_TAG, "--> Handle message: what=[%d], str=[%s]", msg->what, priv->str);
}

static void msg_free(struct message *msg)
{
    struct priv_data *priv = msg->data;
    OS_LOGD(LOG_TAG, "--> Free message: what=[%d], str=[%s]", msg->what, priv->str);
    OS_FREE(priv->str);
    OS_FREE(msg->data);
}

static void msg_notify(struct message *msg, enum message_status status)
{
    if (status == MESSAGE_TIMEOUT)
        OS_LOGE(LOG_TAG, "--> Notify message: what=[%d] timeout", msg->what);
}

int main()
{
    struct os_threadattr attr;
    looper_t looper;
    struct message *msg;
    struct priv_data *priv;

    attr.name = "looper_test_thread";
    attr.priority = OS_THREAD_PRIO_NORMAL;
    attr.stacksize = 1024;
    looper = looper_create(&attr, msg_handle, msg_free);

    looper_dump(looper);

    looper_start(looper);

    {
        priv = OS_MALLOC(sizeof(struct  priv_data));
        priv->str = OS_STRDUP("looper_post_message_delay");
        // timeout 2s
        msg = message_obtain2(1000, 0, 0, priv, 2000, NULL, NULL, msg_notify);
        looper_post_message_delay(looper, msg, 2000); // delay 2s
    }

    {
        for (int i = 0; i < 3; i++) {
            priv = OS_MALLOC(sizeof(struct  priv_data));
            priv->str = OS_STRDUP("looper_post_message");
            msg = message_obtain(i+100, 0, 0, priv);
            looper_post_message(looper, msg);
        }
    }

    {
        priv = OS_MALLOC(sizeof(struct  priv_data));
        priv->str = OS_STRDUP("looper_post_message_front");
        msg = message_obtain(0, 0, 0, priv);
        looper_post_message_front(looper, msg);
    }

    looper_dump(looper);

    //OS_LOGI(LOG_TAG, "remove what=1000");
    //looper_remove_message(looper, 1000);
    //looper_dump(looper);

    //looper_stop(looper);
    //looper_dump(looper);

    OS_THREAD_SLEEP_MSEC(3000);
    looper_destroy(looper);

    return 0;
}
