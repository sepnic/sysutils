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

#ifndef __MSGUTILS_OS_TIME_H__
#define __MSGUTILS_OS_TIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct os_realtime {
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int msec;
};

// utc clock_time, return unix timestamp
unsigned long OS_TIMESTAMP_TO_UTC(struct os_realtime *rt);
// local clock_time, return unix timestamp
unsigned long OS_TIMESTAMP_TO_LOCAL(struct os_realtime *rt);

// monotonictime: clock that cannot be set and represents monotonic time since system bootup
unsigned long long OS_MONOTONIC_USEC();
// realtime: system-wide clock that measures real time (utc timestamp) since 1970.1.1-00:00:00
unsigned long long OS_REALTIME_USEC();

#ifdef __cplusplus
}
#endif

#endif /* __MSGUTILS_OS_TIME_H__ */
