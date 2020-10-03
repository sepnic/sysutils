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

#include <string.h>
#include <unistd.h>
#include <time.h>
#include "cutils/os_time.h"

unsigned long OS_TIMESTAMP_TO_UTC(struct os_realtime *rt)
{
    struct tm now;
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    gmtime_r(&ts.tv_sec, &now);

    if (rt != NULL) {
        rt->year = now.tm_year + 1900;
        rt->mon  = now.tm_mon + 1;
        rt->day  = now.tm_mday;
        rt->hour = now.tm_hour;
        rt->min  = now.tm_min;
        rt->sec  = now.tm_sec;
        rt->msec = (ts.tv_nsec / 1000000) % 1000;
    }
    return ts.tv_sec;
}

unsigned long OS_TIMESTAMP_TO_LOCAL(struct os_realtime *rt)
{
    struct tm now;
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    localtime_r(&ts.tv_sec, &now);

    if (rt != NULL) {
        rt->year = now.tm_year + 1900;
        rt->mon  = now.tm_mon + 1;
        rt->day  = now.tm_mday;
        rt->hour = now.tm_hour;
        rt->min  = now.tm_min;
        rt->sec  = now.tm_sec;
        rt->msec = (ts.tv_nsec / 1000000) % 1000;
    }
    return ts.tv_sec;
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
