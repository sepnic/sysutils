```
.
├── CMakeLists.txt
├── cutils.h
├── include
│   ├── common_list.h
│   ├── msglooper.h
│   ├── os_logger.h
│   ├── os_memory.h
│   ├── os_thread.h
│   ├── os_time.h
│   ├── os_timer.h
│   ├── smartptr.h
│   └── stllist.h
├── LICENSE
├── README.md
├── source
│   ├── msglooper.c
│   ├── memory_detect.c
│   ├── portable
│   │   ├── os_logger.c
│   │   ├── os_thread.c
│   │   ├── os_time.c
│   │   └── os_timer.c
│   ├── smartptr.c
│   ├── stllist.c
│   ├── sw_timer.c
│   ├── sw_timer.h
│   ├── sw_watchdog.c
│   └── sw_watchdog.h
└── test
    ├── CMakeLists.txt
    ├── looper_test.c
    ├── memorydetect_test.c
    ├── smartptr_test.c
    ├── stllist_test.c
    ├── timer_test.c
    └── time_test.c
```

- **stllist**:      list container is similar to stl list
- **common_list**:  linux/android list
- **smartptr**:     smart pointer for c
- **msglooper**:    thread looper to handle message
- **sw_timer**:     software timer
- **sw_watchdog**:  software watchdog
- **memory_detect**:light weight utils to detect memory leak and overflow
- **os_logger**:    log utils to format and save log [portable]
- **os_thread**:    thread interfaces that platform dependent [portable]
- **os_time**:      time interfaces that platform dependent [portable]
- **os_timer**:     timer interfaces that platform dependent [portable]

