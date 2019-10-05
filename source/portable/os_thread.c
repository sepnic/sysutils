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

#include <string.h>
#include "msglooper/os_thread.h"

#if defined(OS_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "FreeRTOS_POSIX/unistd.h"
#include "FreeRTOS_POSIX/time.h"
#include "FreeRTOS_POSIX/pthread.h"
#else
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#endif

void OS_THREAD_SLEEP_USEC(unsigned long usec)
{
    usleep(usec);
}

void OS_THREAD_SLEEP_MSEC(unsigned long msec)
{
    usleep(msec * 1000);
}

#if defined(OS_FREERTOS)
static int OS_THREAD_GET_PRIORITY(enum os_threadprio prio)
{
    // FIXME: Config task priority on your platform
    switch (prio) {
    case OS_THREAD_PRIO_HARD_REALTIME:
        return TASK_PRIORITY_HARD_REALTIME;
    case OS_THREAD_PRIO_SOFT_REALTIME:
        return TASK_PRIORITY_SOFT_REALTIME;
    case OS_THREAD_PRIO_HIGH:
        return TASK_PRIORITY_HIGH;
    case OS_THREAD_PRIO_ABOVE_NORMAL:
        return TASK_PRIORITY_ABOVE_NORMAL;
    case OS_THREAD_PRIO_NORMAL:
        return TASK_PRIORITY_NORMAL;
    case OS_THREAD_PRIO_BELOW_NORMAL:
        return TASK_PRIORITY_BELOW_NORMAL;
    case OS_THREAD_PRIO_LOW:
        return TASK_PRIORITY_LOW;
    case OS_THREAD_PRIO_IDLE:
        return TASK_PRIORITY_IDLE;
    default:
        return TASK_PRIORITY_NORMAL;
    }
}
#endif

os_thread_t OS_THREAD_CREATE(struct os_threadattr *attr, void *(*cb)(void *arg), void *arg)
{
    pthread_t tid;
    int ret;

#if defined(OS_FREERTOS)
    pthread_attr_t tattr;
    struct sched_param tsched;
    int detachstate;

    pthread_attr_init(&tattr);
    tsched.sched_priority = OS_THREAD_GET_PRIORITY(attr->priority);
    pthread_attr_setschedparam(&tattr, &tsched);
    pthread_attr_setstacksize(&tattr, attr->stacksize);
    detachstate = attr->joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED;
    pthread_attr_setdetachstate(&tattr, detachstate);

    ret = pthread_create(&tid, &tattr, cb, arg);
#else
    ret = pthread_create(&tid, NULL, cb, arg);
#endif

    if (ret != 0)
        return NULL;
    return (os_thread_t)tid;
}

void OS_THREAD_CANCEL(os_thread_t tid)
{
    pthread_cancel((pthread_t)tid);
}

int OS_THREAD_JOIN(os_thread_t tid, void **retval)
{
    return pthread_join((pthread_t)tid, retval);
}

os_thread_t OS_THREAD_SELF()
{
    return (os_thread_t)pthread_self();
}

int OS_THREAD_SET_NAME(os_thread_t tid, const char *name)
{
#if defined(OS_FREERTOS)
    return pthread_setname_np((pthread_t)tid, name);
#else
    // TODO
    return -1;
#endif
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
