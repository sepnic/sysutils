LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TOP_DIR := ${LOCAL_PATH}/../..

LOCAL_SRC_FILES := \
    ${TOP_DIR}/source/cutils/memory_debug.c \
    ${TOP_DIR}/source/cutils/msglooper.c \
    ${TOP_DIR}/source/cutils/msgqueue.c \
    ${TOP_DIR}/source/cutils/ringbuf.c \
    ${TOP_DIR}/source/cutils/sw_timer.c \
    ${TOP_DIR}/source/cutils/sw_watchdog.c \
    ${TOP_DIR}/osal/os_logger.c \
    ${TOP_DIR}/osal/os_thread.c \
    ${TOP_DIR}/osal/os_time.c \
    ${TOP_DIR}/osal/os_timer.c \
    ${TOP_DIR}/source/utils/Looper.cpp \
    ${TOP_DIR}/source/utils/Thread.cpp \
    ${TOP_DIR}/source/utils/cJSON.cpp \
    ${TOP_DIR}/source/utils/JsonWrapper.cpp \
    ${TOP_DIR}/source/utils/RefBase.cpp \
    ${TOP_DIR}/source/utils/SharedBuffer.cpp \
    ${TOP_DIR}/source/utils/SafeIop.cpp \
    ${TOP_DIR}/source/utils/VectorImpl.cpp \
    ${TOP_DIR}/source/utils/Unicode.cpp \
    ${TOP_DIR}/source/utils/Static.cpp \
    ${TOP_DIR}/source/utils/String8.cpp \
    ${TOP_DIR}/source/utils/String16.cpp \
    ${TOP_DIR}/source/utils/StringUtils.cpp \
    ${TOP_DIR}/source/cipher/sha2.c \
    ${TOP_DIR}/source/cipher/hmac_sha2.c \
    ${TOP_DIR}/source/cipher/md5.c \
    ${TOP_DIR}/source/cipher/base64.cpp

LOCAL_C_INCLUDES += ${TOP_DIR}/include

LOCAL_CFLAGS += -Wall -Werror -DOS_ANDROID
LOCAL_CPPFLAGS += -Wall -Werror -DOS_ANDROID -std=c++11

LOCAL_LDLIBS := -llog

LOCAL_MODULE := libsysutils

include $(BUILD_SHARED_LIBRARY)
