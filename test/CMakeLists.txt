cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME MACOSX)

set(TOP_DIR "${CMAKE_SOURCE_DIR}/..")

# include files
include_directories(${TOP_DIR}/include)

# source files
file(GLOB_RECURSE COMMON_SRC src "${TOP_DIR}/source/*.c"
                                 "${TOP_DIR}/source/portable/*.c"
)

# cflags
set(CMAKE_C_FLAGS "-DENABLE_DEBUG -g -DOS_MACOSX -Wall -Werror")

# ENABLE_MEMORY_LEAK_DETECT
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_MEMORY_LEAK_DETECT -DENABLE_MEMORY_OVERFLOW_DETECT")

# ENABLE_SMARTPTR_DETECT
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_SMARTPTR_DETECT")

# ldflags
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")

# cutils lib
add_library(cutils SHARED ${COMMON_SRC})
add_library(cutils_s STATIC ${COMMON_SRC})

# stllist test
add_executable(stllist_test ${CMAKE_SOURCE_DIR}/stllist_test.c)
target_link_libraries(stllist_test cutils pthread)
#target_link_libraries(stllist_test rt)

# looper test
add_executable(looper_test ${CMAKE_SOURCE_DIR}/looper_test.c)
target_link_libraries(looper_test cutils pthread)
#target_link_libraries(looper_test rt)

# memorydetect test
add_executable(memorydetect_test ${CMAKE_SOURCE_DIR}/memorydetect_test.c)
target_link_libraries(memorydetect_test cutils pthread)
#target_link_libraries(memorydetect_test rt)

# time test
add_executable(time_test ${CMAKE_SOURCE_DIR}/time_test.c)
target_link_libraries(time_test cutils pthread)
#target_link_libraries(time_test rt)

# timer test
add_executable(timer_test ${CMAKE_SOURCE_DIR}/timer_test.c)
target_link_libraries(timer_test cutils pthread)
#target_link_libraries(timer_test rt)

# smartptr test
add_executable(smartptr_test ${CMAKE_SOURCE_DIR}/smartptr_test.c)
target_link_libraries(smartptr_test cutils pthread)
#target_link_libraries(smartptr_test rt)
