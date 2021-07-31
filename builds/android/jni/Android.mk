LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TOP_DIR := ${LOCAL_PATH}/../../..

LOCAL_SRC_FILES := \
    ${TOP_DIR}/osal/unix/os_log.c \
    ${TOP_DIR}/osal/unix/os_memory.c \
    ${TOP_DIR}/osal/unix/os_thread.c \
    ${TOP_DIR}/osal/unix/os_time.c \
    ${TOP_DIR}/osal/unix/os_timer.c \
    ${TOP_DIR}/source/cutils/memdbg.c \
    ${TOP_DIR}/source/cutils/mlooper.c \
    ${TOP_DIR}/source/cutils/mqueue.c \
    ${TOP_DIR}/source/cutils/ringbuf.c \
    ${TOP_DIR}/source/cutils/swtimer.c \

LOCAL_C_INCLUDES += ${TOP_DIR}/include

LOCAL_CFLAGS += -Wall -Werror -DOS_ANDROID
LOCAL_CPPFLAGS += -Wall -Werror -DOS_ANDROID -std=c++11

LOCAL_LDLIBS := -llog

LOCAL_MODULE := libsysutils

include $(BUILD_SHARED_LIBRARY)
