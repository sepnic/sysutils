cmake_minimum_required(VERSION 3.4.1)
project(msgutils)

set(TOP_DIR "${CMAKE_SOURCE_DIR}")

# include files
include_directories(${TOP_DIR}/include)

# source files
set (LIBS_SRC ${TOP_DIR}/source/cutils/memory_debug.c
              ${TOP_DIR}/source/cutils/msglooper.c
              ${TOP_DIR}/source/cutils/msgqueue.c
              ${TOP_DIR}/source/cutils/ringbuf.c
              ${TOP_DIR}/source/cutils/sw_timer.c
              ${TOP_DIR}/source/cutils/sw_watchdog.c
              ${TOP_DIR}/osal/os_logger.c
              ${TOP_DIR}/osal/os_thread.c
              ${TOP_DIR}/osal/os_time.c
              ${TOP_DIR}/osal/os_timer.c
              ${TOP_DIR}/source/utils/class_debug.cpp
              ${TOP_DIR}/source/utils/Looper.cpp
              ${TOP_DIR}/source/utils/Thread.cpp
              ${TOP_DIR}/source/utils/cJSON.cpp
              ${TOP_DIR}/source/utils/JsonWrapper.cpp
              ${TOP_DIR}/source/utils/RefBase.cpp
              ${TOP_DIR}/source/utils/SharedBuffer.cpp
              ${TOP_DIR}/source/utils/safe_iop.c
              ${TOP_DIR}/source/utils/VectorImpl.cpp
              ${TOP_DIR}/source/utils/Unicode.cpp
              ${TOP_DIR}/source/utils/Static.cpp
              ${TOP_DIR}/source/utils/String8.cpp
              ${TOP_DIR}/source/utils/String16.cpp
)

# cflags: OS_LINUX, OS_ANDROID, OS_MACOSX, OS_IOS, OS_FREERTOS
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Werror -DOS_MACOSX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror -DOS_MACOSX")

# ldflags
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")

# ENABLE_MEMORY_LEAK_DETECT
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DENABLE_MEMORY_LEAK_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_MEMORY_LEAK_DETECT")

# ENABLE_MEMORY_OVERFLOW_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -DENABLE_MEMORY_OVERFLOW_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_MEMORY_OVERFLOW_DETECT")

# ENABLE_SMARTPTR_LEAK_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -DENABLE_SMARTPTR_LEAK_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_SMARTPTR_LEAK_DETECT")

# ENABLE_CLASS_LEAK_DETECT
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_CLASS_LEAK_DETECT")

# msgutils lib
add_library(msgutils   SHARED ${LIBS_SRC})
add_library(msgutils_s STATIC ${LIBS_SRC})
