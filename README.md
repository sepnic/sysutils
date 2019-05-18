```
.
├── CMakeLists.txt
├── cutils.h
├── include
│   ├── common_list.h
│   ├── looper.h
│   ├── os_logger.h
│   ├── os_memory.h
│   ├── os_thread.h
│   ├── os_time.h
│   ├── os_timer.h
│   ├── smart_ptr.h
│   └── stl_list.h
├── LICENSE
├── README.md
├── source
│   ├── looper.c
│   ├── memory_detect.c
│   ├── portable
│   │   ├── os_logger.c
│   │   ├── os_thread.c
│   │   ├── os_time.c
│   │   └── os_timer.c
│   ├── smart_ptr.c
│   ├── stl_list.c
│   ├── sw_timer.c
│   ├── sw_timer.h
│   ├── sw_watchdog.c
│   └── sw_watchdog.h
└── test
    ├── CMakeLists.txt
    ├── looper_test.c
    ├── memory_detect_test.c
    ├── smart_ptr_test.c
    ├── stl_list_test.c
    ├── timer_test.c
    └── time_test.c
```

- **stl_list**:     list container is similar to stl list
- **common_list**:  linux/android list
- **smart_ptr**:    smart pointer for c
- **looper**:       thread looper to handle message
- **sw_timer**:     software timer
- **sw_watchdog**:  software watchdog
- **memory_detect**:light weight utils to detect memory leak and overflow
- **os_logger**:    log utils to format and save log [portable]
- **os_thread**:    thread interfaces that platform dependent [portable]
- **os_time**:      time interfaces that platform dependent [portable]
- **os_timer**:     timer interfaces that platform dependent [portable]

