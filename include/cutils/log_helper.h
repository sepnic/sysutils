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

#ifndef __SYSUTILS_LOG_HELPER_H__
#define __SYSUTILS_LOG_HELPER_H__

#include "osal/os_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(OS_ANDROID)
    #define OS_LOG_CORLOR_FORMAT(letter, format) format
#else
    #define OS_LOG_BLACK         "\033[0;30m"
    #define OS_LOG_RED           "\033[0;31m"
    #define OS_LOG_GREEN         "\033[0;32m"
    #define OS_LOG_BROWN         "\033[0;33m"
    #define OS_LOG_BLUE          "\033[0;34m"
    #define OS_LOG_PURPLE        "\033[0;35m"
    #define OS_LOG_CYAN          "\033[0;36m"
    #define OS_LOG_GRAY          "\033[1;30m"
    #define OS_LOG_COLOR_RESET   "\033[0m"
    #define OS_LOG_COLOR_F       OS_LOG_RED
    #define OS_LOG_COLOR_E       OS_LOG_RED
    #define OS_LOG_COLOR_W       OS_LOG_BROWN
    #define OS_LOG_COLOR_I       OS_LOG_GREEN
    #define OS_LOG_COLOR_D       OS_LOG_BLUE
    #define OS_LOG_COLOR_V       OS_LOG_GRAY
    #define OS_LOG_CORLOR_FORMAT(letter, format)  OS_LOG_COLOR_ ## letter format OS_LOG_COLOR_RESET
#endif

#define OS_LOGF(tag, format, ...) \
    os_fatal(tag, OS_LOG_CORLOR_FORMAT(F, format), ##__VA_ARGS__)
#define OS_LOGE(tag, format, ...) \
    os_error(tag, OS_LOG_CORLOR_FORMAT(E, format), ##__VA_ARGS__)
#define OS_LOGW(tag, format, ...) \
    os_warning(tag, OS_LOG_CORLOR_FORMAT(W, format), ##__VA_ARGS__)
#define OS_LOGI(tag, format, ...) \
    os_info(tag, OS_LOG_CORLOR_FORMAT(I, format), ##__VA_ARGS__)
#define OS_LOGD(tag, format, ...) \
    os_debug(tag, OS_LOG_CORLOR_FORMAT(D, format), ##__VA_ARGS__)
#define OS_LOGV(tag, format, ...) \
    os_verbose(tag, OS_LOG_CORLOR_FORMAT(V, format), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __SYSUTILS_LOG_HELPER_H__ */
