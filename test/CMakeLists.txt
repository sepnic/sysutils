cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME MACOSX)

set(TOP_DIR "${CMAKE_SOURCE_DIR}/..")

# include files
include_directories(${TOP_DIR})

# source files
file(GLOB_RECURSE COMMON_SRC src "${TOP_DIR}/source/*.c"
                                 "${TOP_DIR}/source/portable/*.c"
)

# cflags
set(CMAKE_C_FLAGS "-DENABLE_DEBUG -g -DOS_MACOSX -Wall -Werror")

# ENABLE_MEMORY_LEAK_DETECT
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_MEMORY_LEAK_DETECT -DENABLE_MEMORY_OVERFLOW_DETECT")

# ENABLE_SMART_PTR_DETECT
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_SMART_PTR_DETECT")

# ldflags
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")

# cutils lib
add_library(cutils SHARED ${COMMON_SRC})
add_library(cutils_s STATIC ${COMMON_SRC})

# clist test
add_executable(stl_list_test ${CMAKE_SOURCE_DIR}/stl_list_test.c)
target_link_libraries(stl_list_test cutils pthread)
#target_link_libraries(stl_list_test rt)

# looper test
add_executable(looper_test ${CMAKE_SOURCE_DIR}/looper_test.c)
target_link_libraries(looper_test cutils pthread)
#target_link_libraries(looper_test rt)

# memory detect test
add_executable(memory_detect_test ${CMAKE_SOURCE_DIR}/memory_detect_test.c)
target_link_libraries(memory_detect_test cutils pthread)
#target_link_libraries(memory_detect_test rt)

# time test
add_executable(time_test ${CMAKE_SOURCE_DIR}/time_test.c)
target_link_libraries(time_test cutils pthread)
#target_link_libraries(time_test rt)

# timer test
add_executable(timer_test ${CMAKE_SOURCE_DIR}/timer_test.c)
target_link_libraries(timer_test cutils pthread)
#target_link_libraries(timer_test rt)

# smart ptr test
add_executable(smart_ptr_test ${CMAKE_SOURCE_DIR}/smart_ptr_test.c)
target_link_libraries(smart_ptr_test cutils pthread)
#target_link_libraries(smart_ptr_test rt)
