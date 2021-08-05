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

#ifndef __SYSUTILS_CPP_UTILS_COMMON_H__
#define __SYSUTILS_CPP_UTILS_COMMON_H__

#include <assert.h>
#include "osal/os_common.h"
#include "osal/os_thread.h"
#include "osal/os_time.h"
#include "osal/os_timer.h"
#include "cutils/memory_helper.h"
#include "cutils/log_helper.h"

#define NAMESPACE_BEGIN(name) namespace name {
#define NAMESPACE_END()       }
#define NAMESPACE_USING(name) using namespace name;

#define SYSUTILS_NAMESPACE_BEGIN NAMESPACE_BEGIN(sysutils)
#define SYSUTILS_NAMESPACE_END   NAMESPACE_END()
#define SYSUTILS_NAMESPACE_USING NAMESPACE_USING(sysutils)

#define OS_ASSERT(cond, tag, format, ...) \
    if (!(cond)) { OS_LOGF(tag, format, ##__VA_ARGS__); assert(cond); }

#define OS_FATAL_IF(cond, tag, format, ...) \
    if (cond) { OS_LOGF(tag, format, ##__VA_ARGS__); assert(!(cond)); }

#endif /* __SYSUTILS_CPP_UTILS_COMMON_H__ */
