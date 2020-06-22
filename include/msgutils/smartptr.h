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

#ifndef __MSGUTILS_SMARTPTR_H__
#define __MSGUTILS_SMARTPTR_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define ENABLE_SMARTPTR_LEAK_DETECT

#if !defined(ENABLE_SMARTPTR_LEAK_DETECT)
    void *smartptr_new(size_t size, void (*free_cb)(void *ptr));
    void smartptr_get(void *ptr);
    void smartptr_put(void *ptr);

    #define SMARTPTR_NEW(size, free_cb) smartptr_new((size), (free_cb))
    #define SMARTPTR_GET(ptr) smartptr_get(ptr)
    #define SMARTPTR_PUT(ptr) smartptr_put(ptr)
    #define SMARTPTR_DUMP() do {} while (0)

#else
    void *smartptr_debug_new(size_t size, void (*free_cb)(void *ptr), const char *file, const char *func, int line);
    void smartptr_debug_get(void *ptr, const char *file, const char *func, int line);
    void smartptr_debug_put(void *ptr, const char *file, const char *func, int line);
    void smartptr_debug_dump();

    #define SMARTPTR_NEW(size, free_cb) \
        smartptr_debug_new((size), (free_cb), __FILE__, __FUNCTION__, __LINE__)
    #define SMARTPTR_GET(ptr) \
        smartptr_debug_get(ptr, __FILE__, __FUNCTION__, __LINE__)
    #define SMARTPTR_PUT(ptr) \
        smartptr_debug_put(ptr, __FILE__, __FUNCTION__, __LINE__)
    #define SMARTPTR_DUMP() smartptr_debug_dump()
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MSGUTILS_SMARTPTR_H__ */
