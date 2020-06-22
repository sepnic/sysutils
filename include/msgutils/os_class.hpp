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

#ifndef __MSGUTILS_OS_CLASS_H__
#define __MSGUTILS_OS_CLASS_H__

#include <stdio.h>
#include <stdlib.h>

//#define ENABLE_CLASS_LEAK_DETECT

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

#endif /* __MSGUTILS_OS_CLASS_H__ */
