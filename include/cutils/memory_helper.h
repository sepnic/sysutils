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

#ifndef __SYSUTILS_MEMORY_HELPER_H__
#define __SYSUTILS_MEMORY_HELPER_H__

#include "osal/os_memory.h"

//#define MEMORY_LEAK_DETECT
//#define MEMORY_OVERFLOW_DETECT

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MEMORY_LEAK_DETECT) && !defined(MEMORY_OVERFLOW_DETECT)
    #define OS_MALLOC(size) os_malloc((unsigned int)(size))
    #define OS_CALLOC(n, size) os_calloc((unsigned int)(n), (unsigned int)(size))
    #define OS_REALLOC(ptr, size) os_realloc((void *)(ptr), (unsigned int)(size))
    #define OS_FREE(ptr) do { if (ptr) { os_free((void *)(ptr)); (ptr) = NULL; } } while (0)
    #define OS_STRDUP(str) os_strdup((const char *)(str))
    #define OS_MEMORY_DUMP() do {} while (0)
#else
    void *memdbg_malloc(unsigned int size, const char *file, const char *func, int line);
    void *memdbg_calloc(unsigned int n, unsigned int size, const char *file, const char *func, int line);
    void *memdbg_realloc(void *ptr, unsigned int size, const char *file, const char *func, int line);
    void memdbg_free(void *ptr, const char *file, const char *func, int line);
    char *memdbg_strdup(const char *str, const char *file, const char *func, int line);
    void memdbg_dump_info();

    #define OS_MALLOC(size) \
        memdbg_malloc((unsigned int)(size), __FILE__, __FUNCTION__, __LINE__)
    #define OS_CALLOC(n, size) \
        memdbg_calloc((unsigned int)(n), (unsigned int)(size), __FILE__, __FUNCTION__, __LINE__)
    #define OS_REALLOC(ptr, size) \
        memdbg_realloc((void *)(ptr), (unsigned int)(size), __FILE__, __FUNCTION__, __LINE__)
    #define OS_FREE(ptr) \
        do { if (ptr) { memdbg_free((void *)(ptr), __FILE__, __FUNCTION__, __LINE__); (ptr) = NULL; } } while (0)
    #define OS_STRDUP(str) \
        memdbg_strdup((const char *)(str), __FILE__, __FUNCTION__, __LINE__)
    #define OS_MEMORY_DUMP() \
        memdbg_dump_info()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYSUTILS_MEMORY_HELPER_H__ */
