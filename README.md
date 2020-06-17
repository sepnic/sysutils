```
.
├── CMakeLists.txt
├── LICENSE
├── README.md
├── include
│   └── msgutils
│       ├── common_list.h
│       ├── msglooper.h
│       ├── msgqueue.h
│       ├── os_logger.h
│       ├── os_memory.h
│       ├── os_thread.h
│       ├── os_time.h
│       ├── os_timer.h
│       ├── ringbuf.h
│       ├── smartptr.h
│       ├── sw_timer.h
│       └── sw_watchdog.h
├── port
│   ├── os_logger.c
│   ├── os_thread.c
│   ├── os_time.c
│   └── os_timer.c
├── source
│   ├── memory_detect.c
│   ├── msglooper.c
│   ├── msgqueue.c
│   ├── ringbuf.c
│   ├── smartptr.c
│   ├── sw_timer.c
│   └── sw_watchdog.c
└── test
    ├── CMakeLists.txt
    ├── memorydetect_test.c
    ├── msglooper_test.c
    ├── msgqueue_test.c
    ├── smartptr_test.c
    ├── time_test.c
    └── timer_test.c
```

- **common_list**:  linux/android list
- **smartptr**:     smart pointer for c
- **msglooper**:    thread looper to handle message
- **msgqueue**:     message queue and queue-set
- **ringbuf**:      thread-safe ring buffer
- **sw_timer**:     software timer
- **sw_watchdog**:  software watchdog
- **memory_detect**:light weight utils to detect memory leak and overflow
- **os_logger**:    log utils to format and save log [portable]
- **os_thread**:    thread interfaces that platform dependent [portable]
- **os_time**:      time interfaces that platform dependent [portable]
- **os_timer**:     timer interfaces that platform dependent [portable]

