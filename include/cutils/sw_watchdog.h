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

#ifndef __MSGUTILS_SW_WATCHDOG_H__
#define __MSGUTILS_SW_WATCHDOG_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct swwatchdog_node *swwatch_t;

swwatch_t swwatchdog_create(const char *name, unsigned long long timeout_ms, void (*timeout_cb)(void *arg), void *arg);

int swwatchdog_start(swwatch_t node);

int swwatchdog_feed(swwatch_t node);

int swwatchdog_stop(swwatch_t node);

void swwatchdog_destroy(swwatch_t node);

#ifdef __cplusplus
}
#endif

#endif /* __MSGUTILS_SW_WATCHDOG_H__ */
