LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TOP_DIR := ${LOCAL_PATH}/../../..

LOCAL_SRC_FILES := \
    ${TOP_DIR}/source/memory_debug.c \
    ${TOP_DIR}/source/msglooper.c \
    ${TOP_DIR}/source/msgqueue.c \
    ${TOP_DIR}/source/ringbuf.c \
    ${TOP_DIR}/source/smartptr.c \
    ${TOP_DIR}/source/sw_timer.c \
    ${TOP_DIR}/source/sw_watchdog.c \
    ${TOP_DIR}/osal/os_logger.c \
    ${TOP_DIR}/osal/os_thread.c \
    ${TOP_DIR}/osal/os_time.c \
    ${TOP_DIR}/osal/os_timer.c \
    ${TOP_DIR}/source/class_debug.cpp \
    ${TOP_DIR}/source/Looper.cpp \
    ${TOP_DIR}/source/Thread.cpp \
    ${TOP_DIR}/source/cJSON.cpp \
    ${TOP_DIR}/source/JsonWrapper.cpp

LOCAL_C_INCLUDES += ${TOP_DIR}/include

LOCAL_CFLAGS += -Wall -Werror -DOS_ANDROID

LOCAL_LDLIBS := -llog

LOCAL_MODULE := libmsgutils

include $(BUILD_SHARED_LIBRARY)
