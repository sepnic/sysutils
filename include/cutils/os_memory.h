/*
 * Copyright (C) 2018-2020 luoyun <sysu.zqlong@gmail.com>
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

#ifndef __MSGUTILS_OS_MEMORY_H__
#define __MSGUTILS_OS_MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//#define ENABLE_MEMORY_LEAK_DETECT
//#define ENABLE_MEMORY_OVERFLOW_DETECT
//#define ENABLE_CLASS_LEAK_DETECT

// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ENABLE_MEMORY_OVERFLOW_DETECT)
void *memory_debug_malloc(size_t size, const char *file, const char *func, int line, bool overflow_detect);
void *memory_debug_calloc(size_t n, size_t size, const char *file, const char *func, int line, bool overflow_detect);
void *memory_debug_realloc(void *ptr, size_t size, const char *file, const char *func, int line, bool overflow_detect);
void memory_debug_free(void *ptr, const char *file, const char *func, int line, bool overflow_detect);
char *memory_debug_strdup(const char *str, const char *file, const char *func, int line, bool overflow_detect);
void memory_debug_dump(bool overflow_detect);

    #define OS_MALLOC(size) \
        memory_debug_malloc((size_t)(size), __FILE__, __FUNCTION__, __LINE__, true)
    #define OS_CALLOC(n, size) \
        memory_debug_calloc((size_t)(n), (size_t)(size), __FILE__, __FUNCTION__, __LINE__, true)
    #define OS_REALLOC(ptr, size) \
        memory_debug_realloc((void *)(ptr), (size_t)(size), __FILE__, __FUNCTION__, __LINE__, true)
    #define OS_FREE(ptr) \
        do {\
            if (ptr) {\
                memory_debug_free((void *)(ptr), __FILE__, __FUNCTION__, __LINE__, true);\
                (ptr) = NULL;\
            }\
        } while (0)
    #define OS_STRDUP(str) memory_debug_strdup((const char *)(str), __FILE__, __FUNCTION__, __LINE__, true)
    #define OS_STREQUAL(str1, str2) (((str1) && (str2)) ? (strcmp((str1), (str2)) == 0) : false)
    #define OS_MEMORY_DUMP() memory_debug_dump(true)

#elif defined(ENABLE_MEMORY_LEAK_DETECT)
void *memory_debug_malloc(size_t size, const char *file, const char *func, int line, bool overflow_detect);
void *memory_debug_calloc(size_t n, size_t size, const char *file, const char *func, int line, bool overflow_detect);
void *memory_debug_realloc(void *ptr, size_t size, const char *file, const char *func, int line, bool overflow_detect);
void memory_debug_free(void *ptr, const char *file, const char *func, int line, bool overflow_detect);
char *memory_debug_strdup(const char *str, const char *file, const char *func, int line, bool overflow_detect);
void memory_debug_dump(bool overflow_detect);

    #define OS_MALLOC(size) \
        memory_debug_malloc((size_t)(size), __FILE__, __FUNCTION__, __LINE__, false)
    #define OS_CALLOC(n, size) \
        memory_debug_calloc((size_t)(n), (size_t)(size), __FILE__, __FUNCTION__, __LINE__, false)
    #define OS_REALLOC(ptr, size) \
        memory_debug_realloc((void *)(ptr), (size_t)(size), __FILE__, __FUNCTION__, __LINE__, false)
    #define OS_FREE(ptr) \
        do {\
            if (ptr) {\
                memory_debug_free((void *)(ptr), __FILE__, __FUNCTION__, __LINE__, false);\
                (ptr) = NULL;\
            }\
        } while (0)
    #define OS_STRDUP(str) memory_debug_strdup((const char *)(str), __FILE__, __FUNCTION__, __LINE__, false)
    #define OS_STREQUAL(str1, str2) (((str1) && (str2)) ? (strcmp((str1), (str2)) == 0) : false)
    #define OS_MEMORY_DUMP() memory_debug_dump(false)

#else
char *memory_strdup(const char *str);

    #define OS_MALLOC(size) malloc((size_t)(size))
    #define OS_CALLOC(n, size) calloc((size_t)(n), (size_t)(size))
    #define OS_REALLOC(ptr, size) realloc((void *)(ptr), (size_t)(size))
    #define OS_FREE(ptr) \
        do {\
            if (ptr) {\
                free((void *)(ptr));\
                (ptr) = NULL;\
            }\
        } while (0)
    #define OS_STRDUP(str) memory_strdup((const char *)(str))
    #define OS_STREQUAL(str1, str2) (((str1) && (str2)) ? (strcmp((str1), (str2)) == 0) : false)
    #define OS_MEMORY_DUMP() do {} while (0)
#endif

// ---------------------------------------------------------------------------

#if !defined(ENABLE_CLASS_LEAK_DETECT)
    #define OS_NEW(ptr, Class, ...)        ptr = new Class(__VA_ARGS__)
    #define OS_DELETE(ptr) \
        do {\
            if (ptr) {\
                delete ptr;\
                (ptr) = NULL;\
            }\
        } while (0)

    #define OS_NEW_ARRAY(ptr, Class, size) ptr = new Class[size]
    #define OS_DELETE_ARRAY(ptr) \
        do {\
            if (ptr) {\
                delete [] ptr;\
                (ptr) = NULL;\
            }\
        } while (0)

    #define OS_CLASS_DUMP() do {} while (0)

#else
    void class_debug_new(void *ptr, const char *name, const char *file, const char *func, int line);
    void class_debug_delete(void *ptr, const char *file, const char *func, int line);
    void class_debug_dump();

    #define OS_NEW(ptr, Class, ...) \
        do {\
            ptr = new Class(__VA_ARGS__);\
            class_debug_new((void *)ptr, #Class, __FILE__, __FUNCTION__, __LINE__);\
        } while (0)
    #define OS_DELETE(ptr) \
        do {\
            if (ptr) {\
                class_debug_delete((void *)ptr, __FILE__, __FUNCTION__, __LINE__);\
                delete ptr;\
                (ptr) = NULL;\
            }\
        } while (0)

    #define OS_NEW_ARRAY(ptr, Class, size) \
        do {\
            ptr = new Class[size] \
            class_debug_new((void *)ptr, #Class, __FILE__, __FUNCTION__, __LINE__);\
        } while (0)
    #define OS_DELETE_ARRAY(ptr) \
        do {\
            if (ptr) {\
                class_debug_delete((void *)ptr, __FILE__, __FUNCTION__, __LINE__);\
                delete [] ptr;\
                (ptr) = NULL;\
            }\
        } while (0)

    #define OS_CLASS_DUMP() class_debug_dump()
#endif

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------

#endif /* __MSGUTILS_OS_MEMORY_H__ */
