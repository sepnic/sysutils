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

#include <string.h>
#include <stdbool.h>
#include "cutils/common_list.h"
#include "cutils/os_time.h"
#include "cutils/os_logger.h"
#include "cutils/os_memory.h"
#include "cutils/os_thread.h"
#include "utils/os_class.h"

#define LOG_TAG "objdebug"

struct class_node {
    void *ptr;
    const char *name;
    const char *file;
    const char *func;
    int line;
    struct os_realtime when;
    struct listnode listnode;
};

struct class_info {
    struct listnode list;

    long new_cnt;
    long delete_cnt;

    os_mutex_t mutex;
};

OS_MUTEX_DECLARE(g_info_mutex);
static struct class_info *g_info = NULL;

static char *file_name(const char *filepath)
{
    char *filename = (char *)filepath;

    if (filename != NULL) {
        unsigned int len = strlen(filepath);
        if (len > 0) {
            filename += len - 1;
            while (filename > filepath) {
                if ((*filename == '\\') || *filename == '/') {
                    filename++;
                    break;
                }
                filename--;
            }
        }
    }
    return filename;
}

static void class_node_print(struct class_node *node, const char *info)
{
    OS_LOGW(LOG_TAG, "> %s: ptr=[%p], class=[%s], created by [%s:%s:%d], at [%04d%02d%02d-%02d%02d%02d:%03d]",
           info, node->ptr, node->name, file_name(node->file), node->func, node->line,
           node->when.year, node->when.mon, node->when.day,
           node->when.hour, node->when.min, node->when.sec, node->when.msec);
}

static struct class_info *class_debug_init()
{
    if (g_info == NULL) {
        if (g_info_mutex != NULL)
            OS_THREAD_MUTEX_LOCK(g_info_mutex);

        if (g_info == NULL) {
            g_info = (struct class_info *)OS_CALLOC(1, sizeof(struct class_info));
            if (g_info == NULL) {
                OS_LOGE(LOG_TAG, "Failed to alloc class_info, abort class debug");
                if (g_info_mutex != NULL)
                    OS_THREAD_MUTEX_UNLOCK(g_info_mutex);
                return NULL;
            }

            g_info->mutex = OS_THREAD_MUTEX_CREATE();
            if (g_info->mutex == NULL) {
                OS_LOGE(LOG_TAG, "Failed to alloc class_mutex, abort class debug");
                goto error;
            }

            g_info->new_cnt = 0;
            g_info->delete_cnt = 0;
            list_init(&g_info->list);
        }

        if (g_info_mutex != NULL)
            OS_THREAD_MUTEX_UNLOCK(g_info_mutex);
    }

    return g_info;

error:
    if (g_info->mutex != NULL)
        OS_THREAD_MUTEX_DESTROY(g_info->mutex);

    OS_FREE(g_info);
    g_info = NULL;

    if (g_info_mutex != NULL)
        OS_THREAD_MUTEX_UNLOCK(g_info_mutex);
    return NULL;
}

void class_debug_new(void *ptr, const char *name, const char *file, const char *func, int line)
{
    struct class_info *info = class_debug_init();
    if (info != NULL) {
        struct class_node *node = (struct class_node *)OS_MALLOC(sizeof(struct class_node));
        if (node != NULL) {
            node->ptr = ptr;
            node->name = name;
            node->file = file;
            node->func = func;
            node->line = line;
            OS_TIMESTAMP_TO_LOCAL(&node->when);

            //class_node_print(node, "new");

            OS_THREAD_MUTEX_LOCK(info->mutex);
            list_add_tail(&info->list, &node->listnode);
            info->new_cnt++;
            OS_THREAD_MUTEX_UNLOCK(info->mutex);
        }
    }
}

void class_debug_delete(void *ptr, const char *file, const char *func, int line)
{
    struct class_info *info = class_debug_init();
    if (info != NULL) {
        struct class_node *node;
        struct listnode *item;
        bool found = false;

        OS_THREAD_MUTEX_LOCK(info->mutex);

        list_for_each_reverse(item, &info->list) {
            node = node_to_item(item, struct class_node, listnode);
            if (node->ptr == ptr) {
                found = true;
                break;
            }
        }

        if (found) {
            //class_node_print(node, "delete");
            info->delete_cnt++;
            list_remove(item);
            OS_FREE(node);
        }
        else {
            OS_LOGF(LOG_TAG, "%s:%s:%d: failed to find ptr[%p] in list", file_name(file), func, line, ptr);
        }

        OS_THREAD_MUTEX_UNLOCK(info->mutex);
    }
}

void class_debug_dump()
{
    struct class_info *info  = class_debug_init();
    if (info != NULL) {
        OS_LOGW(LOG_TAG, ">>");
        OS_LOGW(LOG_TAG, "++++++++++++++++++++ CLASS DEBUG ++++++++++++++++++++");

        OS_THREAD_MUTEX_LOCK(info->mutex);

        struct listnode *item;
        list_for_each(item, &info->list) {
            struct class_node * node = node_to_item(item, struct class_node, listnode);
            class_node_print(node, "Dump");
        }

        OS_LOGW(LOG_TAG, "Summary: new [%ld] blocks, delete [%ld] blocks", info->new_cnt, info->delete_cnt);

        OS_THREAD_MUTEX_UNLOCK(info->mutex);

        OS_LOGW(LOG_TAG, "-------------------- CLASS DEBUG --------------------");
        OS_LOGW(LOG_TAG, "<<");
    }
}
