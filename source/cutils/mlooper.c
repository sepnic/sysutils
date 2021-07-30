/*
 * Copyright (c) 2018-2021 Qinglong<sysu.zqlong@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include "osal/os_thread.h"
#include "osal/os_time.h"
#include "cutils/memory_helper.h"
#include "cutils/log_helper.h"
#include "cutils/list.h"
#include "cutils/mlooper.h"

#define LOG_TAG "mlooper"

struct mlooper {
    struct listnode msg_list;
    int msg_count;
    message_cb msg_handle;
    message_cb msg_free;
    os_mutex msg_mutex;
    os_cond msg_cond;

    os_thread thread_id;
    const char *thread_name;
    struct os_thread_attr thread_attr;
    bool thread_exit;
    os_mutex thread_mutex;
};

struct message_node {
    // @msg must be the first member of message_node, as users of this structure
    // will cast a message to message_node pointer in contexts where it's known
    // the message references a message_node
    struct message msg;
    unsigned long long when;
    unsigned long long timeout;
    os_thread owner_thread;
    struct listnode listnode;
    char reserve[0];
};

static void mlooper_free_msgnode(mlooper_handle looper, struct message_node *node)
{
    struct message *msg = &node->msg;
    if (msg->free_cb != NULL)
        msg->free_cb(msg);
    else if (looper->msg_free != NULL)
        looper->msg_free(msg);
    OS_FREE(node);
}

static void mlooper_clear_msglist(mlooper_handle looper)
{
    struct message_node *node = NULL;
    struct listnode *item, *tmp;

    os_mutex_lock(looper->msg_mutex);

    list_for_each_safe(item, tmp, &looper->msg_list) {
        node = listnode_to_item(item, struct message_node, listnode);
        list_remove(item);
        mlooper_free_msgnode(looper, node);
    }
    looper->msg_count = 0;

    os_mutex_unlock(looper->msg_mutex);
}

static void *mlooper_thread_entry(void *arg)
{
    struct mlooper *looper = (struct mlooper *)arg;
    struct message_node *node = NULL;
    struct message *msg = NULL;
    struct listnode *front = NULL;
    unsigned long long now;

    OS_LOGD(LOG_TAG, "[%s]: Entry looper thread: thread_id=[%p]",
            looper->thread_name, looper->thread_id);

    while (1) {
        {
            os_mutex_lock(looper->msg_mutex);

            while (list_empty(&looper->msg_list) && !looper->thread_exit)
                os_cond_wait(looper->msg_cond, looper->msg_mutex);

            if (looper->thread_exit) {
                os_mutex_unlock(looper->msg_mutex);
                break;
            }

            front = list_head(&looper->msg_list);
            node = listnode_to_item(front, struct message_node, listnode);
            msg = &node->msg;

            now = os_monotonic_usec();
            if (node->when > now) {
                unsigned long wait = node->when - now;
                OS_LOGV(LOG_TAG, "[%s]: Waiting message: what=[%d], wait=[%lums]",
                        looper->thread_name, msg->what, wait/1000);
                os_cond_timedwait(looper->msg_cond, looper->msg_mutex, wait);
                msg = NULL;
            } else {
                list_remove(front);
                looper->msg_count--;
            }

            os_mutex_unlock(looper->msg_mutex);
        }

        if (msg != NULL) {
            if (node->timeout > 0 && node->timeout < now) {
                OS_LOGE(LOG_TAG, "[%s]: Timeout, discard message: what=[%d]",
                        looper->thread_name, msg->what);
                if (msg->timeout_cb != NULL)
                    msg->timeout_cb(msg);
            } else {
                if (msg->handle_cb != NULL)
                    msg->handle_cb(msg);
                else if (looper->msg_handle != NULL)
                    looper->msg_handle(msg);
                else
                    OS_LOGW(LOG_TAG, "[%s]: No message handler: what=[%d]",
                            looper->thread_name, msg->what);
            }
            mlooper_free_msgnode(looper, node);
        }
    }

    mlooper_clear_msglist(looper);

    OS_LOGD(LOG_TAG, "[%s]: Leave looper thread: thread_id=[%p]",
            looper->thread_name, looper->thread_id);
    return NULL;
}

mlooper_handle mlooper_create(struct os_thread_attr *attr, message_cb handle_cb, message_cb free_cb)
{
    struct mlooper *looper = OS_CALLOC(1, sizeof(struct mlooper));
    if (looper == NULL) {
        OS_LOGE(LOG_TAG, "Failed to allocate looper");
        return NULL;
    }

    looper->msg_mutex = os_mutex_create();
    if (looper->msg_mutex == NULL) {
        OS_LOGE(LOG_TAG, "Failed to create msg_mutex");
        goto error;
    }

    looper->msg_cond = os_cond_create();
    if (looper->msg_cond == NULL) {
        OS_LOGE(LOG_TAG, "Failed to create msg_cond");
        goto error;
    }

    looper->thread_mutex = os_mutex_create();
    if (looper->thread_mutex == NULL) {
        OS_LOGE(LOG_TAG, "Failed to create thread_mutex");
        goto error;
    }

    list_init(&looper->msg_list);
    looper->msg_count = 0;
    looper->msg_handle = handle_cb;
    looper->msg_free = free_cb;
    looper->thread_name = (attr && attr->name) ? OS_STRDUP(attr->name) : OS_STRDUP("mlooper");
    looper->thread_exit = true;
    looper->thread_attr.name = looper->thread_name;
    if (attr != NULL) {
        looper->thread_attr.priority = attr->priority;
        looper->thread_attr.stacksize =
            attr->stacksize > 0 ? attr->stacksize : os_thread_default_stacksize();
        looper->thread_attr.joinable = true; // force joinalbe, wait exit when mlooper_stop
    } else {
        looper->thread_attr.priority = os_thread_default_priority();
        looper->thread_attr.stacksize = os_thread_default_stacksize();
        looper->thread_attr.joinable = true;
    }
    return looper;

error:
    if (looper->thread_mutex != NULL)
        os_mutex_destroy(looper->thread_mutex);
    if (looper->msg_cond != NULL)
        os_cond_destroy(looper->msg_cond);
    if (looper->msg_mutex != NULL)
        os_mutex_destroy(looper->msg_mutex);
    OS_FREE(looper);
    return NULL;
}

int mlooper_start(mlooper_handle looper)
{
    int ret = 0;
    os_mutex_lock(looper->thread_mutex);

    if (looper->thread_exit) {
        looper->thread_exit = false;
        looper->thread_id = os_thread_create(&(looper->thread_attr), mlooper_thread_entry, looper);
        if (looper->thread_id == NULL) {
            OS_LOGE(LOG_TAG, "[%s]: Failed to run thread looper", looper->thread_name);
            looper->thread_exit = true;
            ret = -1;
        }
    }

    os_mutex_unlock(looper->thread_mutex);
    return ret;
}

int mlooper_post_message(mlooper_handle looper, struct message *msg)
{
    return mlooper_post_message_delay(looper, msg, 0);
}

int mlooper_post_message_front(mlooper_handle looper, struct message *msg)
{
    unsigned long long now = os_monotonic_usec();
    struct message_node *node = (struct message_node *)msg;
    struct message_node *front;

    node->when = now;
    node->owner_thread = os_thread_self();
    if (msg->timeout_ms > 0)
        node->timeout = now + msg->timeout_ms * 1000;

    {
        os_mutex_lock(looper->msg_mutex);

        if (!list_empty(&looper->msg_list)) {
            front = listnode_to_item(list_head(&looper->msg_list), struct message_node, listnode);
            node->when = now < front->when ? now : front->when;
        }
        list_add_head(&looper->msg_list, &node->listnode);
        looper->msg_count++;

        os_cond_signal(looper->msg_cond);

        os_mutex_unlock(looper->msg_mutex);
    }
    return 0;
}

int mlooper_post_message_delay(mlooper_handle looper, struct message *msg, unsigned long msec)
{
    unsigned long long now = os_monotonic_usec();
    struct message_node *node = (struct message_node *)msg;
    struct message_node *temp;
    struct listnode *item;

    node->when = now + msec*1000;
    node->owner_thread = os_thread_self();
    if (msg->timeout_ms > 0) {
        if (msg->timeout_ms <= msec) {
            OS_LOGE(LOG_TAG, "[%s]: timeout_ms <= delay_ms, discard message: what=[%d]",
                    looper->thread_name, msg->what);
            mlooper_free_msgnode(looper, node);
            return -1;
        } else {
            node->timeout = now + msg->timeout_ms * 1000;
        }
    }

    {
        os_mutex_lock(looper->msg_mutex);

        list_for_each_reverse(item, &looper->msg_list) {
            temp = listnode_to_item(item, struct message_node, listnode);
            if (node->when >= temp->when) {
                list_add_after(&temp->listnode, &node->listnode);
                looper->msg_count++;
                break;
            }
        }
        if (item == &looper->msg_list) {
            list_add_head(&looper->msg_list, &node->listnode);
            looper->msg_count++;
        }

        os_cond_signal(looper->msg_cond);

        os_mutex_unlock(looper->msg_mutex);
    }
    return 0;
}

int mlooper_remove_message(mlooper_handle looper, int what)
{
    struct message_node *node = NULL;
    struct listnode *item, *tmp;
    os_thread caller = os_thread_self();

    os_mutex_lock(looper->msg_mutex);

    list_for_each_safe(item, tmp, &looper->msg_list) {
        node = listnode_to_item(item, struct message_node, listnode);
        if (node->msg.what == what && caller == node->owner_thread) {
            list_remove(item);
            mlooper_free_msgnode(looper, node);
            looper->msg_count--;
        }
    }

    os_mutex_unlock(looper->msg_mutex);
    return 0;
}

int mlooper_remove_message_if(mlooper_handle looper, bool (*match_cb)(struct message *msg))
{
    struct message_node *node = NULL;
    struct listnode *item, *tmp;
    os_thread caller = os_thread_self();

    os_mutex_lock(looper->msg_mutex);

    list_for_each_safe(item, tmp, &looper->msg_list) {
        node = listnode_to_item(item, struct message_node, listnode);
        if (match_cb(&node->msg) && caller == node->owner_thread) {
            list_remove(item);
            mlooper_free_msgnode(looper, node);
            looper->msg_count--;
        }
    }

    os_mutex_unlock(looper->msg_mutex);
    return 0;
}

int mlooper_message_count(mlooper_handle looper)
{
    return looper->msg_count;
}

void mlooper_dump(mlooper_handle looper)
{
    struct message_node *node = NULL;
    struct listnode *item;
    int i = 0;

    os_mutex_lock(looper->msg_mutex);

    OS_LOGI(LOG_TAG, "Dump looper thread:");
    OS_LOGI(LOG_TAG, " > thread_name=[%s]", looper->thread_name);
    OS_LOGI(LOG_TAG, " > thread_exit=[%s]", looper->thread_exit ? "true" : "false");
    OS_LOGI(LOG_TAG, " > message_count=[%d]", looper->msg_count);

    if (looper->msg_count != 0) {
        OS_LOGI(LOG_TAG, " > message list info:");
        list_for_each(item, &looper->msg_list) {
            node = listnode_to_item(item, struct message_node, listnode);
            i++;
            OS_LOGI(LOG_TAG, "   > [%d]: owner=[%p], what=[%d], arg1=[%d], arg2=[%d], when=[%llu]",
                    i, node->owner_thread, node->msg.what, node->msg.arg1, node->msg.arg2, node->when);
        }
    }

    os_mutex_unlock(looper->msg_mutex);
}

void mlooper_stop(mlooper_handle looper)
{
    if (looper->thread_id == os_thread_self()) {
        OS_LOGW(LOG_TAG,
                "Thread (%p:%s): don't call mlooper_stop() from this thread. Maybe deadlock!",
                looper->thread_id, looper->thread_name);
    }

    os_mutex_lock(looper->thread_mutex);
    if (!looper->thread_exit) {
        os_mutex_lock(looper->msg_mutex);
        looper->thread_exit = true;
        os_cond_signal(looper->msg_cond);
        os_mutex_unlock(looper->msg_mutex);

        os_thread_join(looper->thread_id, NULL);
    }
    os_mutex_unlock(looper->thread_mutex);
}

void mlooper_destroy(mlooper_handle looper)
{
    mlooper_stop(looper);

    os_mutex_destroy(looper->thread_mutex);
    os_cond_destroy(looper->msg_cond);
    os_mutex_destroy(looper->msg_mutex);

    OS_FREE(looper->thread_name);
    OS_FREE(looper);
}

struct message *message_obtain(int what, int arg1, int arg2, void *data)
{
    struct message *msg = OS_CALLOC(1, sizeof(struct message_node));
    if (msg == NULL) {
        OS_LOGE(LOG_TAG, "Failed to allocate message");
        return NULL;
    }
    msg->what = what;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->data = data;
    return msg;
}

struct message *message_obtain_buffer_obtain(int what, int arg1, int arg2, unsigned int size)
{
    unsigned int total = sizeof(struct message_node) + size + sizeof(long);
    struct message_node *node = OS_CALLOC(1, total);
    if (node == NULL) {
        OS_LOGE(LOG_TAG, "Failed to allocate message");
        return NULL;
    }
    struct message *msg = (struct message *)node;
    msg->what = what;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->data = node->reserve;
    return msg;
}

void message_set_handle_cb(struct message *msg, message_cb handle_cb)
{
    msg->handle_cb = handle_cb;
}

void message_set_free_cb(struct message *msg, message_cb free_cb)
{
    msg->free_cb = free_cb;
}

void message_set_timeout_cb(struct message *msg, message_cb timeout_cb, unsigned long timeout_ms)
{
    msg->timeout_cb = timeout_cb;
    msg->timeout_ms = timeout_ms;
}
