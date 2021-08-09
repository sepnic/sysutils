cmake_minimum_required(VERSION 3.4.1)
project(sysutils)

set(TOP_DIR "${CMAKE_SOURCE_DIR}")

# include files
include_directories(${TOP_DIR}/include)

# source files
set (LIBS_SRC
    ${TOP_DIR}/osal/unix/os_log.c
    ${TOP_DIR}/osal/unix/os_memory.c
    ${TOP_DIR}/osal/unix/os_thread.c
    ${TOP_DIR}/osal/unix/os_time.c
    ${TOP_DIR}/osal/unix/os_timer.c
    ${TOP_DIR}/osal/unix/os_misc.c
    ${TOP_DIR}/source/cutils/memdbg.c
    ${TOP_DIR}/source/cutils/mlooper.c
    ${TOP_DIR}/source/cutils/mqueue.c
    ${TOP_DIR}/source/cutils/ringbuf.c
    ${TOP_DIR}/source/cutils/swtimer.c
    ${TOP_DIR}/source/utils/Looper.cpp
    ${TOP_DIR}/source/utils/Thread.cpp
    ${TOP_DIR}/source/utils/cJSON.cpp
    ${TOP_DIR}/source/utils/JsonWrapper.cpp
    ${TOP_DIR}/source/utils/RefBase.cpp
    ${TOP_DIR}/source/utils/SharedBuffer.cpp
    ${TOP_DIR}/source/utils/SafeIop.cpp
    ${TOP_DIR}/source/utils/VectorImpl.cpp
    ${TOP_DIR}/source/utils/Unicode.cpp
    ${TOP_DIR}/source/utils/Static.cpp
    ${TOP_DIR}/source/utils/String8.cpp
    ${TOP_DIR}/source/utils/String16.cpp
    ${TOP_DIR}/source/utils/StringUtils.cpp
    ${TOP_DIR}/source/cipher/sha2.c
    ${TOP_DIR}/source/cipher/hmac_sha2.c
    ${TOP_DIR}/source/cipher/md5.c
    ${TOP_DIR}/source/cipher/base64.c
)

# cflags: OS_LINUX, OS_ANDROID, OS_APPLE, OS_RTOS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -DOS_APPLE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -DOS_APPLE")

# MEMORY_LEAK_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMORY_LEAK_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMEMORY_LEAK_DETECT)

# MEMORY_OVERFLOW_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMORY_OVERFLOW_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMEMORY_OVERFLOW_DETECT")

# sysutils lib
add_library(sysutils   SHARED ${LIBS_SRC})
add_library(sysutils_s STATIC ${LIBS_SRC})
