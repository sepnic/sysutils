cmake_minimum_required(VERSION 2.8)

if (TARGET_DEFINITION)
    message("including ${TARGET_DEFINITION}")
    include(${TARGET_DEFINITION})
endif ()

# include files
include_directories(${CMAKE_SOURCE_DIR})

# source files
file(GLOB_RECURSE COMMON_SRC src "${CMAKE_SOURCE_DIR}/source/*.c"
                                 "${CMAKE_SOURCE_DIR}/source/portable/*.c"
)

# cflags: OS_LINUX, OS_ANDROID, OS_MACOSX, OS_IOS, OS_FREERTOS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Werror -DOS_MACOSX")

# ldflags
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")

# ENABLE_MEMORY_LEAK_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_MEMORY_LEAK_DETECT")

# ENABLE_MEMORY_OVERFLOW_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_MEMORY_OVERFLOW_DETECT")

# ENABLE_SMART_PTR_DETECT
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_SMART_PTR_DETECT")

message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")

# cutils lib
add_library(cutils SHARED ${COMMON_SRC})
add_library(cutils_s STATIC ${COMMON_SRC})
