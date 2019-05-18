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
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "include/os_thread.h"

#if defined(OS_LINUX) || defined(OS_ANDROID) || defined(OS_MACOSX) || defined(OS_IOS)

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void OS_ENTER_CRITICAL()
{
    pthread_mutex_lock(&g_mutex);
}

void OS_LEAVE_CRITICAL()
{
    pthread_mutex_unlock(&g_mutex);
}

void OS_THREAD_SLEEP_USEC(unsigned long usec)
{
    usleep(usec);
}

void OS_THREAD_SLEEP_MSEC(unsigned long msec)
{
    usleep(msec * 1000);
}

os_thread_t OS_THREAD_CREATE(struct os_threadattr *attr, void *(*cb)(void *arg), void *arg)
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, cb, arg);
    if (ret != 0)
        return NULL;
    return (os_thread_t)tid;
}

void OS_THREAD_DESTROY(os_thread_t tid)
{

}

void OS_THREAD_WAIT_EXIT(os_thread_t tid)
{
    pthread_join((pthread_t)tid, NULL);
}

os_mutex_t OS_THREAD_MUTEX_CREATE()
{
    pthread_mutex_t *mutex = calloc(1, sizeof(pthread_mutex_t));
    if (mutex == NULL) return NULL;

    pthread_mutex_init(mutex, NULL);
    return (os_mutex_t)mutex;
}

int OS_THREAD_MUTEX_LOCK(os_mutex_t mutex)
{
    return pthread_mutex_lock((pthread_mutex_t *)mutex);
}

int OS_THREAD_MUTEX_TRYLOCK(os_mutex_t mutex)
{
    return pthread_mutex_trylock((pthread_mutex_t *)mutex);
}

int OS_THREAD_MUTEX_UNLOCK(os_mutex_t mutex)
{
    return pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

void OS_THREAD_MUTEX_DESTROY(os_mutex_t mutex)
{
    pthread_mutex_destroy((pthread_mutex_t *)mutex);
    free(mutex);
}

os_cond_t OS_THREAD_COND_CREATE()
{
    pthread_cond_t *cond = calloc(1, sizeof(pthread_cond_t));
    if (cond == NULL) return NULL;

    pthread_cond_init(cond, NULL);
    return (os_cond_t)cond;
}

int OS_THREAD_COND_WAIT(os_cond_t cond, os_mutex_t mutex)
{
    return pthread_cond_wait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex);
}

int OS_THREAD_COND_TIMEDWAIT(os_cond_t cond, os_mutex_t mutex, unsigned long usec)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += usec / 1000000;
    ts.tv_nsec += (usec % 1000000) * 1000;
    while (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec++;
        ts.tv_nsec %= 1000000000L;
    }

    return pthread_cond_timedwait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex, &ts);
}

int OS_THREAD_COND_SIGNAL(os_cond_t cond)
{
    return pthread_cond_signal((pthread_cond_t *)cond);
}

void OS_THREAD_COND_DESTROY(os_cond_t cond)
{
    pthread_cond_destroy((pthread_cond_t *)cond);
    free(cond);
}

#elif defined(OS_FREERTOS)

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "task_def.h"

void OS_ENTER_CRITICAL()
{
    taskENTER_CRITICAL();
}

void OS_LEAVE_CRITICAL()
{
    taskEXIT_CRITICAL();
}

void OS_THREAD_SLEEP_USEC(unsigned long usec)
{
    long int msec = usec/1000;
    msec = msec > 0 ? msec : 1;
    vTaskDelay(msec/portTICK_PERIOD_MS);
}

void OS_THREAD_SLEEP_MSEC(unsigned long msec)
{
    vTaskDelay(msec/portTICK_PERIOD_MS);
}

os_thread_t OS_THREAD_CREATE(struct os_threadattr *attr, void *(*cb)(void *arg), void *arg)
{
    TaskHandle_t tid = NULL;
    BaseType_t ret;
    task_priority_type_t prio = TASK_PRIORITY_NORMAL;

    if (attr == NULL)
        return NULL;

    switch (attr->priority) {
    case OS_THREAD_PRIO_HARD_REALTIME:
        prio = TASK_PRIORITY_HARD_REALTIME;
        break;
    case OS_THREAD_PRIO_SOFT_REALTIME:
        prio = TASK_PRIORITY_SOFT_REALTIME;
        break;
    case OS_THREAD_PRIO_HIGH:
        prio = TASK_PRIORITY_HIGH;
        break;
    case OS_THREAD_PRIO_ABOVE_NORMAL:
        prio = TASK_PRIORITY_ABOVE_NORMAL;
        break;
    case OS_THREAD_PRIO_NORMAL:
        prio = TASK_PRIORITY_NORMAL;
        break;
    case OS_THREAD_PRIO_BELOW_NORMAL:
        prio = TASK_PRIORITY_BELOW_NORMAL;
        break;
    case OS_THREAD_PRIO_LOW:
        prio = TASK_PRIORITY_LOW;
        break;
    case OS_THREAD_PRIO_IDLE:
        prio = TASK_PRIORITY_IDLE;
        break;
    default:
        prio = TASK_PRIORITY_NORMAL;
        break;
    }

    ret = xTaskCreate((pdTASK_CODE)cb, attr->name, attr->stacksize/sizeof(StackType_t), arg, prio, &tid);
    return ret == pdPASS ? (os_thread_t)tid : NULL;
}

void OS_THREAD_DESTROY(os_thread_t tid)
{
    vTaskDelete((TaskHandle_t)tid);
}

void OS_THREAD_WAIT_EXIT(os_thread_t tid)
{
    while (eTaskGetState((TaskHandle_t)tid) != eDeleted)
        vTaskDelay(1/portTICK_PERIOD_MS);
}

os_mutex_t OS_THREAD_MUTEX_CREATE()
{
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    return (os_mutex_t)mutex;
}

int OS_THREAD_MUTEX_LOCK(os_mutex_t mutex)
{
    BaseType_t ret = xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY);
    return ret == pdTRUE ? 0 : -1;
}

int OS_THREAD_MUTEX_TRYLOCK(os_mutex_t mutex)
{
    BaseType_t ret = ret = xSemaphoreTake((SemaphoreHandle_t)mutex, wait);
    return ret == pdTRUE ? 0 : -1;
}

int OS_THREAD_MUTEX_UNLOCK(os_mutex_t mutex)
{
    BaseType_t ret = xSemaphoreGive((SemaphoreHandle_t)mutex);
    return ret == pdTRUE ? 0 : -1;
}

void OS_THREAD_MUTEX_DESTROY(os_mutex_t mutex)
{
    vSemaphoreDelete((SemaphoreHandle_t)mutex);
}

os_cond_t OS_THREAD_COND_CREATE()
{
    SemaphoreHandle_t cond = xSemaphoreCreateBinary();
    return (os_cond_t)cond;
}

int OS_THREAD_COND_WAIT(os_cond_t cond, os_mutex_t mutex)
{
    BaseType_t ret = pdFALSE;

    OS_THREAD_MUTEX_UNLOCK(mutex);

    ret = xSemaphoreTake((SemaphoreHandle_t)cond, portMAX_DELAY);

    OS_THREAD_MUTEX_LOCK(mutex);

    return ret == pdTRUE ? 0 : -1;
}

int OS_THREAD_COND_TIMEDWAIT(os_cond_t cond, os_mutex_t mutex, unsigned long usec)
{
    BaseType_t ret = pdFALSE;
    TickType_t wait = usec/1000/portTICK_PERIOD_MS;

    wait = wait > 0 ? wait : 1;

    OS_THREAD_MUTEX_UNLOCK(mutex);

    ret = xSemaphoreTake((SemaphoreHandle_t)cond, wait);

    OS_THREAD_MUTEX_LOCK(mutex);

    return ret == pdTRUE ? 0 : -1;
}

int OS_THREAD_COND_SIGNAL(os_cond_t cond)
{
    BaseType_t ret = xSemaphoreGive((SemaphoreHandle_t)cond);
    return ret == pdTRUE ? 0 : -1;
}

void OS_THREAD_COND_DESTROY(os_cond_t cond)
{
    vSemaphoreDelete((SemaphoreHandle_t)cond);
}

#endif // #if defined(OS_FREERTOS)
