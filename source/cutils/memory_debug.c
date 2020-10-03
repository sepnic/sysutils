/* The MIT License (MIT)
 *
 * Copyright (c) 2019 luoyun <sysu.zqlong@gmail.com>
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

#include <stdbool.h>
#include "cutils/common_list.h"
#include "cutils/os_thread.h"
#include "cutils/os_time.h"
#include "cutils/os_logger.h"
#include "cutils/os_memory.h"

#define LOG_TAG "memdebug"

// ---------------------------------------------------------------------------

#define MEMORY_BOUNDARY_SIZE    8
#define MEMORY_BOUNDARY_FLAG    '*'

struct mem_node {
    void *ptr;
    size_t size;
    const char *file;
    const char *func;
    int line;
    struct os_realtime when;
    struct listnode listnode;
};

struct mem_info {
    struct listnode list;

    long malloc_count;
    long free_count;
    size_t cur_used;
    size_t max_used;

    os_mutex_t mutex;
};

OS_MUTEX_DECLARE(g_meminfo_mutex);
static struct mem_info *g_meminfo = NULL;

void *memory_debug_malloc(size_t size, const char *file, const char *func, int line, bool overflow_detect);
void *memory_debug_calloc(size_t n, size_t size, const char *file, const char *func, int line, bool overflow_detect);
void *memory_debug_realloc(void *ptr, size_t size, const char *file, const char *func, int line, bool overflow_detect);
void memory_debug_free(void *ptr, const char *file, const char *func, int line, bool overflow_detect);
char *memory_debug_strdup(const char *str, const char *file, const char *func, int line, bool overflow_detect);
char *memory_strdup(const char *str);
void memory_debug_dump(bool overflow_detect);

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

static void memory_node_print(struct mem_node *node, const char *info)
{
    OS_LOGW(LOG_TAG, "> %s: ptr=[%p], size=[%lu], "
           "created by [%s:%s:%d], at [%04d%02d%02d-%02d%02d%02d:%03d]",
           info, node->ptr, (unsigned long)node->size,
           file_name(node->file), node->func, node->line,
           node->when.year, node->when.mon, node->when.day,
           node->when.hour, node->when.min, node->when.sec, node->when.msec);
}

static void *memory_boundary_malloc(size_t size)
{
    void *ptr = NULL;
    size_t real_size = size;

    real_size += 2 * MEMORY_BOUNDARY_SIZE;
    ptr = malloc(real_size);

    if (ptr != NULL) {
        // fill upper boundary with specific flag
        memset(ptr,
               MEMORY_BOUNDARY_FLAG,
               MEMORY_BOUNDARY_SIZE);
        // fill lower boundary with specific flag
        memset((char *)ptr + real_size - MEMORY_BOUNDARY_SIZE,
               MEMORY_BOUNDARY_FLAG,
               MEMORY_BOUNDARY_SIZE);

        return (char *)ptr + MEMORY_BOUNDARY_SIZE;
    }
    return NULL;
}

static void memory_boundary_free(void *ptr)
{
    if (ptr != NULL) {
        void *temp = (void *) ((char *)ptr - MEMORY_BOUNDARY_SIZE);
        free(temp);
    }
}

static void memory_boundary_verify(struct mem_node *node)
{
    int i = 0;
    char *ptr;

    // check lower boundary
    ptr = (char *)node->ptr - MEMORY_BOUNDARY_SIZE;
    for (i = 0; i < MEMORY_BOUNDARY_SIZE; i++) {
        if (((char *)ptr)[i] != MEMORY_BOUNDARY_FLAG) {
            memory_node_print(node, "Overflow at lower boundary");
            break;
        }
    }

    // check upper boundary
    ptr = (char *)node->ptr + node->size;
    for (i = 0; i < MEMORY_BOUNDARY_SIZE; i++) {
        if (((char *)ptr)[i] != MEMORY_BOUNDARY_FLAG) {
            memory_node_print(node, "Overflow at upper boundary");
            break;
        }
    }
}

static struct mem_info *memory_debug_init()
{
    if (g_meminfo == NULL) {
        if (g_meminfo_mutex != NULL)
            OS_THREAD_MUTEX_LOCK(g_meminfo_mutex);

        if (g_meminfo == NULL) {
            g_meminfo = calloc(1, sizeof(struct mem_info));
            if (g_meminfo == NULL) {
                OS_LOGE(LOG_TAG, "Failed to alloc mem_info, abort memory debug");
                if (g_meminfo_mutex != NULL)
                    OS_THREAD_MUTEX_UNLOCK(g_meminfo_mutex);
                return NULL;
            }

            g_meminfo->mutex = OS_THREAD_MUTEX_CREATE();
            if (g_meminfo->mutex == NULL) {
                OS_LOGE(LOG_TAG, "Failed to alloc mem_mutex, abort memory debug");
                goto error;
            }

            g_meminfo->malloc_count = 0;
            g_meminfo->free_count = 0;
            g_meminfo->max_used = 0;
            list_init(&g_meminfo->list);
        }

        if (g_meminfo_mutex != NULL)
            OS_THREAD_MUTEX_UNLOCK(g_meminfo_mutex);
    }

    return g_meminfo;

error:
    if (g_meminfo->mutex != NULL)
        OS_THREAD_MUTEX_DESTROY(g_meminfo->mutex);

    free(g_meminfo);
    g_meminfo = NULL;

    if (g_meminfo_mutex != NULL)
        OS_THREAD_MUTEX_UNLOCK(g_meminfo_mutex);
    return NULL;
}

void *memory_debug_malloc(size_t size, const char *file, const char *func, int line, bool overflow_detect)
{
    void *ptr;
    struct mem_node *node;
    struct mem_info *info = memory_debug_init();

    if (overflow_detect)
        ptr = memory_boundary_malloc(size);
    else
        ptr = malloc(size);

    if (ptr == NULL) {
        OS_LOGF(LOG_TAG, "%s:%s:%d: failed to alloc memory", file_name(file), func, line);
        return NULL;
    }

    if (info != NULL) {
        node = malloc(sizeof(struct mem_node));
        if (node != NULL) {
            node->ptr = ptr;
            node->size = size;
            node->file = file;
            node->func = func;
            node->line = line;
            OS_TIMESTAMP_TO_LOCAL(&node->when);

            //memory_node_print(node, "Malloc");

            OS_THREAD_MUTEX_LOCK(info->mutex);

            list_add_tail(&info->list, &node->listnode);
            info->malloc_count++;
            info->cur_used += size;
            if (info->cur_used > info->max_used)
                info->max_used = info->cur_used;

            OS_THREAD_MUTEX_UNLOCK(info->mutex);
        }
    }

    return ptr;
}

void *memory_debug_calloc(size_t n, size_t size, const char *file, const char *func, int line, bool overflow_detect)
{
    void *ptr = memory_debug_malloc(size * n, file, func, line, overflow_detect);
    if (ptr != NULL)
        memset(ptr, 0x0, size * n);
    return ptr;
}

void *memory_debug_realloc(void *ptr, size_t size, const char *file, const char *func, int line, bool overflow_detect)
{
    struct mem_info *info;
    struct listnode *item;

    if (ptr == NULL) {
        if (size > 0)
            return memory_debug_malloc(size, file, func, line, overflow_detect);
        else
            return NULL;
    }

    if (size == 0) {
        memory_debug_free(ptr, file, func, line, overflow_detect);
        return NULL;
    }

    info = memory_debug_init();

    if (info != NULL) {
        void *prev_ptr = NULL;
        size_t prev_size = 0;

        OS_THREAD_MUTEX_LOCK(info->mutex);

        list_for_each_reverse(item, &info->list) {
            struct mem_node *node = node_to_item(item, struct mem_node, listnode);
            if (node->ptr == ptr) {
                prev_ptr = node->ptr;
                prev_size = node->size;
                break;
            }
        }

        if (prev_ptr == NULL) {
            OS_LOGF(LOG_TAG, "%s:%s:%d: failed to find ptr[%p] in list, abort realloc",
                   file_name(file), func, line, ptr);
            return NULL;
        }

        OS_THREAD_MUTEX_UNLOCK(info->mutex);

        if (size > prev_size) {
            void *new_ptr = memory_debug_malloc(size, file, func, line, overflow_detect);
            if (new_ptr != NULL)
                memcpy(new_ptr, prev_ptr, prev_size);

            memory_debug_free(prev_ptr, file, func, line, overflow_detect);
            return new_ptr;
        }
        else
            return prev_ptr;
    }
    else {
        if (overflow_detect) {
            OS_LOGF(LOG_TAG, "%s:%s:%d: failed to create mem_info instance, abort realloc",
                    file_name(file), func, line);
            return NULL;
        }
        else {
            return realloc(ptr, size);
        }
    }

    return NULL;
}

void memory_debug_free(void *ptr, const char *file, const char *func, int line, bool overflow_detect)
{
    struct mem_info *info = memory_debug_init();
    struct listnode *item;

    if (info != NULL) {
        struct mem_node *node;
        bool found = false;

        OS_THREAD_MUTEX_LOCK(info->mutex);

        list_for_each_reverse(item, &info->list) {
            node = node_to_item(item, struct mem_node, listnode);
            if (node->ptr == ptr) {
                found = true;
                break;
            }
        }

        if (found) {
            if (overflow_detect)
                memory_boundary_verify(node);

            //memory_node_print(node, "Free");

            info->free_count++;
            info->cur_used -= node->size;

            list_remove(item);
            free(node);
        }
        else {
            OS_LOGF(LOG_TAG, "%s:%s:%d: failed to find ptr[%p] in list, double free?",
                   file_name(file), func, line, ptr);
        }

        OS_THREAD_MUTEX_UNLOCK(info->mutex);
    }

    // free memory
    if (overflow_detect)
        memory_boundary_free(ptr);
    else
        free(ptr);
}

char *memory_debug_strdup(const char *str, const char *file, const char *func, int line, bool overflow_detect)
{
    char *ptr;
    size_t len;

    if (str == NULL)
        return NULL;

    len = strlen(str);
    ptr = memory_debug_malloc(len + 1, file, func, line, overflow_detect);
    if (ptr != NULL) {
        memcpy(ptr, str, len);
        ptr[len] = '\0';
    }

    return ptr;
}

char *memory_strdup(const char *str)
{
    if (str == NULL)
        return NULL;

    size_t len = strlen(str);
    char *ptr = (char *)malloc(len + 1);
    if (ptr != NULL) {
        memcpy(ptr, str, len);
        ptr[len] = '\0';
    }

    return ptr;
}

void memory_debug_dump(bool overflow_detect)
{
    struct mem_info *info  = memory_debug_init();
    struct mem_node *node;
    struct listnode *item;

    if (info != NULL) {
        OS_LOGW(LOG_TAG, ">>");
        OS_LOGW(LOG_TAG, "++++++++++++++++++++ MEMORY DEBUG ++++++++++++++++++++");

        OS_THREAD_MUTEX_LOCK(info->mutex);

        list_for_each(item, &info->list) {
            node = node_to_item(item, struct mem_node, listnode);
            memory_node_print(node, "Dump");
            if (overflow_detect)
                memory_boundary_verify(node);
        }

        OS_LOGW(LOG_TAG, "Summary: malloc [%ld] blocks, free [%ld] blocks, current use [%lu] Bytes, max use [%lu] Bytes",
                info->malloc_count, info->free_count, (unsigned long)info->cur_used, (unsigned long)info->max_used);

        OS_THREAD_MUTEX_UNLOCK(info->mutex);

        OS_LOGW(LOG_TAG, "-------------------- MEMORY DEBUG --------------------");
        OS_LOGW(LOG_TAG, "<<");
    }
}

// ---------------------------------------------------------------------------

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

OS_MUTEX_DECLARE(g_clzinfo_mutex);
static struct class_info *g_clzinfo = NULL;

static void class_node_print(struct class_node *node, const char *info)
{
    OS_LOGW(LOG_TAG, "> %s: ptr=[%p], class=[%s], created by [%s:%s:%d], at [%04d%02d%02d-%02d%02d%02d:%03d]",
           info, node->ptr, node->name, file_name(node->file), node->func, node->line,
           node->when.year, node->when.mon, node->when.day,
           node->when.hour, node->when.min, node->when.sec, node->when.msec);
}

static struct class_info *class_debug_init()
{
    if (g_clzinfo == NULL) {
        if (g_clzinfo_mutex != NULL)
            OS_THREAD_MUTEX_LOCK(g_clzinfo_mutex);

        if (g_clzinfo == NULL) {
            g_clzinfo = (struct class_info *)OS_CALLOC(1, sizeof(struct class_info));
            if (g_clzinfo == NULL) {
                OS_LOGE(LOG_TAG, "Failed to alloc class_info, abort class debug");
                if (g_clzinfo_mutex != NULL)
                    OS_THREAD_MUTEX_UNLOCK(g_clzinfo_mutex);
                return NULL;
            }

            g_clzinfo->mutex = OS_THREAD_MUTEX_CREATE();
            if (g_clzinfo->mutex == NULL) {
                OS_LOGE(LOG_TAG, "Failed to alloc class_mutex, abort class debug");
                goto error;
            }

            g_clzinfo->new_cnt = 0;
            g_clzinfo->delete_cnt = 0;
            list_init(&g_clzinfo->list);
        }

        if (g_clzinfo_mutex != NULL)
            OS_THREAD_MUTEX_UNLOCK(g_clzinfo_mutex);
    }

    return g_clzinfo;

error:
    if (g_clzinfo->mutex != NULL)
        OS_THREAD_MUTEX_DESTROY(g_clzinfo->mutex);

    OS_FREE(g_clzinfo);
    g_clzinfo = NULL;

    if (g_clzinfo_mutex != NULL)
        OS_THREAD_MUTEX_UNLOCK(g_clzinfo_mutex);
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

// ---------------------------------------------------------------------------
