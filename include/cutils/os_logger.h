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

#ifndef __MSGUTILS_OS_LOGGER_H__
#define __MSGUTILS_OS_LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

enum os_logprio {
    OS_LOG_FATAL = 0,
    OS_LOG_ERROR,
    OS_LOG_WARN,
    OS_LOG_INFO,
    OS_LOG_DEBUG,
    OS_LOG_VERBOSE,
};

void os_logger_config(bool enable, enum os_logprio prio);

// [date] [time] [pid] [tid] [prio] [tag]:[func]:[line]: [log]
void os_logger_trace(enum os_logprio prio, const char *tag, const char *func, unsigned int line,
                     const char *format, ...);

#if defined(OS_ANDROID)
    #include <android/log.h>
    #define OS_LOGF(tag, format, ...) __android_log_print(ANDROID_LOG_FATAL, tag, format, ##__VA_ARGS__)
    #define OS_LOGE(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
    #define OS_LOGW(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)
    #define OS_LOGI(tag, format, ...) __android_log_print(ANDROID_LOG_INFO, tag, format, ##__VA_ARGS__)
    #define OS_LOGD(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)
    #define OS_LOGV(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)

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
    #define OS_LOG_COLOR_V       "\033[1;30m"
    #define OS_LOG_FORMAT(letter, format)  OS_LOG_COLOR_ ## letter format OS_LOG_COLOR_RESET

    #define OS_LOGF(tag, format, ...) \
        os_logger_trace(OS_LOG_FATAL, tag, __FUNCTION__, __LINE__, OS_LOG_FORMAT(F, format), ##__VA_ARGS__)
    #define OS_LOGE(tag, format, ...) \
        os_logger_trace(OS_LOG_ERROR, tag, __FUNCTION__, __LINE__, OS_LOG_FORMAT(E, format), ##__VA_ARGS__)
    #define OS_LOGW(tag, format, ...) \
        os_logger_trace(OS_LOG_WARN, tag, __FUNCTION__, __LINE__, OS_LOG_FORMAT(W, format), ##__VA_ARGS__)
    #define OS_LOGI(tag, format, ...) \
        os_logger_trace(OS_LOG_INFO, tag, __FUNCTION__, __LINE__, OS_LOG_FORMAT(I, format), ##__VA_ARGS__)
    #define OS_LOGD(tag, format, ...) \
        os_logger_trace(OS_LOG_DEBUG, tag, __FUNCTION__, __LINE__, OS_LOG_FORMAT(D, format), ##__VA_ARGS__)
    #define OS_LOGV(tag, format, ...) \
        os_logger_trace(OS_LOG_VERBOSE, tag, __FUNCTION__, __LINE__, OS_LOG_FORMAT(V, format), ##__VA_ARGS__)
#endif

#if 1
    #define OS_ASSERT(cond, tag, format, ...)\
            if (!(cond)) { OS_LOGF(tag, format, ##__VA_ARGS__); assert(cond); }
#else
    #define OS_ASSERT(cond, tag, format, ...) do {} while (0);
#endif

    #define OS_FATAL_IF(cond, tag, format, ...)\
            if (cond) { OS_LOGF(tag, format, ##__VA_ARGS__); assert(!(cond)); }

#ifdef __cplusplus
}
#endif

#endif /* __MSGUTILS_OS_LOGGER_H__ */
