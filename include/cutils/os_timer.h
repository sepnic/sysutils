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

#ifndef __MSGUTILS_OS_TIMER_H__
#define __MSGUTILS_OS_TIMER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *os_timer_t;

struct os_timerattr {
    const char *name;        // name is assigned to the timer, purely to assist debugging
    unsigned long period_ms; // the timer period in milliseconds
    bool reload;             // if reload set to true then the timer will expire repeatedly
                             // if reload set to false then the timer will be a one-shot timer
};

os_timer_t OS_TIMER_CREATE(struct os_timerattr *attr, void (*cb)());

int OS_TIMER_START(os_timer_t timer);

int OS_TIMER_STOP(os_timer_t timer);

bool OS_TIMER_IS_ACTIVE(os_timer_t timer);

void OS_TIMER_DESTROY(os_timer_t timer);

#ifdef __cplusplus
}
#endif

#endif /* __MSGUTILS_OS_TIMER_H__ */
