#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "cutils/memory_helper.h"
#include "cutils/log_helper.h"
#include "cutils/mlooper.h"

#define LOG_TAG "msglooper_test"

struct priv_data {
    const char *str;
};

static void msg_handle(struct message *arg)
{
    struct message *msg = arg;
    OS_LOGD(LOG_TAG, "--> Handle message: what=[%d], str=[%s]", msg->what, msg->data);
}

static void msg_free(struct message *msg)
{
    OS_LOGD(LOG_TAG, "--> Free message: what=[%d], str=[%s]", msg->what, msg->data);
    OS_FREE(msg->data);
}

static void msg_handle2(struct message *arg)
{
    struct message *msg = arg;
    struct priv_data *priv = msg->data;
    OS_LOGD(LOG_TAG, "--> Handle message: what=[%d], str=[%s]", msg->what, priv->str);
}

static void msg_free2(struct message *msg)
{
    struct priv_data *priv = msg->data;
    OS_LOGD(LOG_TAG, "--> Free message: what=[%d], str=[%s]", msg->what, priv->str);
    OS_FREE(priv->str);
}

static void msg_timeout(struct message *msg)
{
    OS_LOGE(LOG_TAG, "--> Timeout message: what=[%d]", msg->what);
}

int main()
{
    struct os_thread_attr attr;
    mlooper_handle looper;
    struct message *msg;
    const char *str;

    attr.name = "msglooper_test";
    attr.priority = os_thread_default_priority();
    attr.stacksize = 1024;
    looper = mlooper_create(&attr, msg_handle, msg_free);

    mlooper_dump(looper);

    mlooper_start(looper);

    {
        str = OS_STRDUP("mlooper_post_message_delay");
        // timeout 2s
        msg = message_obtain(1000, 0, 0, (void *)str);
        message_set_timeout_cb(msg, msg_timeout, 2000);
        mlooper_post_message_delay(looper, msg, 2000); // delay 2s
    }

    {
        for (int i = 0; i < 3; i++) {
            str = OS_STRDUP("mlooper_post_message");
            msg = message_obtain(i+100, 0, 0, (void *)str);
            mlooper_post_message(looper, msg);
        }
    }

    {
        struct priv_data priv;
        priv.str = OS_STRDUP("mlooper_post_message_front");
        msg = message_obtain_copy_data(0, 0, 0, &priv, sizeof(priv));
        message_set_handle_cb(msg, msg_handle2);
        message_set_free_cb(msg, msg_free2);
        mlooper_post_message_front(looper, msg);
    }

    mlooper_dump(looper);

    //OS_LOGI(LOG_TAG, "remove what=1000");
    //mlooper_remove_message(looper, 1000);
    //mlooper_dump(looper);

    os_thread_sleep_msec(3000);
    mlooper_destroy(looper);
    return 0;
}
