cmake_minimum_required(VERSION 3.4.1)
project(sysutils_test)

set(TOP_DIR "${CMAKE_SOURCE_DIR}/..")

# include files
include_directories(${TOP_DIR}/include)

# source files
set (LIBS_SRC
    ${TOP_DIR}/osal/unix/os_log.c
    ${TOP_DIR}/osal/unix/os_memory.c
    ${TOP_DIR}/osal/unix/os_thread.c
    ${TOP_DIR}/osal/unix/os_time.c
    ${TOP_DIR}/osal/unix/os_timer.c
    ${TOP_DIR}/source/cutils/memdbg.c
    ${TOP_DIR}/source/cutils/swtimer.c
)

# cflags: OS_LINUX, OS_ANDROID, OS_APPLE, OS_RTOS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -DOS_APPLE")

# MEMORY_LEAK_DETECT, MEMORY_OVERFLOW_DETECT
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMEMORY_LEAK_DETECT -DMEMORY_OVERFLOW_DETECT")

# sysutils lib
add_library(sysutils   SHARED ${LIBS_SRC})
add_library(sysutils_s STATIC ${LIBS_SRC})

# timer test
add_executable(timer_test ${CMAKE_SOURCE_DIR}/timer_test.c)
target_link_libraries(timer_test sysutils pthread)

# memdbg test
add_executable(memdbg_test ${CMAKE_SOURCE_DIR}/memdbg_test.c)
target_link_libraries(memdbg_test sysutils pthread)
