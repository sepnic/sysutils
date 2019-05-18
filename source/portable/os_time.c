/* The MIT License (MIT)
 *
 * Copyright (c) 2018 luoyun <sysu.zqlong@gmail.com>
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

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "include/os_time.h"

#if defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_MACOSX) || defined(OS_IOS)

unsigned long OS_TIMESTAMP_TO_UTC(struct os_clocktime *tm)
{
    struct tm now;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    gmtime_r(&tv.tv_sec, &now);

    if (tm != NULL) {
        tm->year = now.tm_year + 1900;
        tm->mon  = now.tm_mon + 1;
        tm->day  = now.tm_mday;
        tm->hour = now.tm_hour;
        tm->min  = now.tm_min;
        tm->sec  = now.tm_sec;
        tm->msec = (tv.tv_usec / 1000) % 1000;
    }
    return tv.tv_sec;
}

unsigned long OS_TIMESTAMP_TO_LOCAL(struct os_clocktime *tm)
{
    struct tm now;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &now);

    if (tm != NULL) {
        tm->year = now.tm_year + 1900;
        tm->mon  = now.tm_mon + 1;
        tm->day  = now.tm_mday;
        tm->hour = now.tm_hour;
        tm->min  = now.tm_min;
        tm->sec  = now.tm_sec;
        tm->msec = (tv.tv_usec / 1000) % 1000;
    }
    return tv.tv_sec;
}

unsigned long long OS_MONOTONIC_USEC()
{
    struct timespec ts;
    unsigned long long cputime;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    cputime = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    return cputime;
}

unsigned long long OS_REALTIME_USEC()
{
    struct timespec ts;
    unsigned long long cputime;

    clock_gettime(CLOCK_REALTIME, &ts);

    cputime = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    return cputime;
}

#elif defined(OS_FREERTOS)

#include "FreeRTOS.h"
#include "task.h"

unsigned long OS_TIMESTAMP_TO_UTC(struct os_clocktime *tm)
{
    struct tm now;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    gmtime_r(&tv.tv_sec, &now);

    if (tm != NULL) {
        tm->year = now.tm_year + 1900;
        tm->mon  = now.tm_mon + 1;
        tm->day  = now.tm_mday;
        tm->hour = now.tm_hour;
        tm->min  = now.tm_min;
        tm->sec  = now.tm_sec;
        tm->msec = (tv.tv_usec / 1000) % 1000;
    }
    return tv.tv_sec;
}

unsigned long OS_TIMESTAMP_TO_LOCAL(struct os_clocktime *tm)
{
    struct tm now;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &now);

    if (tm != NULL) {
        tm->year = now.tm_year + 1900;
        tm->mon  = now.tm_mon + 1;
        tm->day  = now.tm_mday;
        tm->hour = now.tm_hour;
        tm->min  = now.tm_min;
        tm->sec  = now.tm_sec;
        tm->msec = (tv.tv_usec / 1000) % 1000;
    }
    return tv.tv_sec;
}

unsigned long long OS_MONOTONIC_USEC()
{
    return (unsigned long long)(xTaskGetTickCount() * portTICK_PERIOD_MS * 1000);
}

unsigned long long OS_REALTIME_USEC()
{
    struct timeval tv;
    unsigned long long realtime;

    gettimeofday(&tv, NULL);
    realtime = tv.tv_sec * 1000000 + tv.tv_usec;

    return realtime;
}

#endif // #if defined(OS_FREERTOS)
