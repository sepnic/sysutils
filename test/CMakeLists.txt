cmake_minimum_required(VERSION 2.8)
project(msgutils_test)

set(CMAKE_SYSTEM_NAME MACOSX)

set(TOP_DIR "${CMAKE_SOURCE_DIR}/..")

# include files
include_directories(${TOP_DIR}/include)

# source files
set (LIBS_SRC ${TOP_DIR}/source/memory_debug.c
              ${TOP_DIR}/source/msglooper.c
              ${TOP_DIR}/source/msgqueue.c
              ${TOP_DIR}/source/ringbuf.c
              ${TOP_DIR}/source/smartptr.c
              ${TOP_DIR}/source/sw_timer.c
              ${TOP_DIR}/source/sw_watchdog.c
              ${TOP_DIR}/osal/os_logger.c
              ${TOP_DIR}/osal/os_thread.c
              ${TOP_DIR}/osal/os_time.c
              ${TOP_DIR}/osal/os_timer.c
              ${TOP_DIR}/source/class_debug.cpp
              ${TOP_DIR}/source/Looper.cpp
              ${TOP_DIR}/source/Thread.cpp
)

# cflags
set(CMAKE_C_FLAGS   "-DENABLE_DEBUG -g -DOS_MACOSX -std=gnu99 -Wall -Werror")
set(CMAKE_CXX_FLAGS "-DENABLE_DEBUG -g -Wall -Werror")

# ENABLE_MEMORY_LEAK_DETECT
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DENABLE_MEMORY_LEAK_DETECT -DENABLE_MEMORY_OVERFLOW_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_MEMORY_LEAK_DETECT -DENABLE_MEMORY_OVERFLOW_DETECT")

# ENABLE_SMARTPTR_LEAK_DETECT
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -DENABLE_SMARTPTR_LEAK_DETECT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_SMARTPTR_LEAK_DETECT")

# ENABLE_CLASS_LEAK_DETECT
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_CLASS_LEAK_DETECT")

# ldflags
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")

# msgutils lib
add_library(msgutils   SHARED ${LIBS_SRC})
add_library(msgutils_s STATIC ${LIBS_SRC})

# msglooper test
add_executable(msglooper ${CMAKE_SOURCE_DIR}/msglooper_main.c)
target_link_libraries(msglooper msgutils pthread)

# msgqueue test
add_executable(msgqueue ${CMAKE_SOURCE_DIR}/msgqueue_main.c)
target_link_libraries(msgqueue msgutils pthread)

# Looper test
add_executable(Looper ${CMAKE_SOURCE_DIR}/Looper_main.cpp)
target_link_libraries(Looper msgutils pthread)

# Thread test
add_executable(Thread ${CMAKE_SOURCE_DIR}/Thread_main.cpp)
target_link_libraries(Thread msgutils pthread)
