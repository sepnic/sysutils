cmake_minimum_required(VERSION 2.8)

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
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -DENABLE_MEMORY_LEAK_DETECT -DENABLE_MEMORY_OVERFLOW_DETECT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_MEMORY_LEAK_DETECT -DENABLE_MEMORY_OVERFLOW_DETECT")

# ENABLE_SMARTPTR_LEAK_DETECT
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -DENABLE_SMARTPTR_LEAK_DETECT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_SMARTPTR_LEAK_DETECT")

# ENABLE_CLASS_LEAK_DETECT
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DENABLE_CLASS_LEAK_DETECT")

# ldflags
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")

# msgutils lib
add_library(msgutils   SHARED ${LIBS_SRC})
add_library(msgutils_s STATIC ${LIBS_SRC})

# msglooper test
add_executable(msglooper ${CMAKE_SOURCE_DIR}/msglooper_test.c)
target_link_libraries(msglooper msgutils pthread)
#target_link_libraries(msglooper rt)

# memorydetect test
add_executable(memorydetect_test ${CMAKE_SOURCE_DIR}/memorydetect_test.c)
target_link_libraries(memorydetect_test msgutils pthread)
#target_link_libraries(memorydetect_test rt)

# time test
add_executable(time_test ${CMAKE_SOURCE_DIR}/time_test.c)
target_link_libraries(time_test msgutils pthread)
#target_link_libraries(time_test rt)

# timer test
add_executable(timer_test ${CMAKE_SOURCE_DIR}/timer_test.c)
target_link_libraries(timer_test msgutils pthread)
#target_link_libraries(timer_test rt)

# smartptr test
add_executable(smartptr_test ${CMAKE_SOURCE_DIR}/smartptr_test.c)
target_link_libraries(smartptr_test msgutils pthread)
#target_link_libraries(smartptr_test rt)

# msgqueue test
add_executable(msgqueue_test ${CMAKE_SOURCE_DIR}/msgqueue_test.c)
target_link_libraries(msgqueue_test msgutils pthread)
#target_link_libraries(msgqueue_test rt)

# Looper test
add_executable(LooperTest ${CMAKE_SOURCE_DIR}/LooperTest.cpp)
target_link_libraries(LooperTest msgutils pthread)
#target_link_libraries(LooperTest rt)

# Thread test
add_executable(ThreadTest ${CMAKE_SOURCE_DIR}/ThreadTest.cpp)
target_link_libraries(ThreadTest msgutils pthread)
#target_link_libraries(ThreadTest rt)
