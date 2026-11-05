list(APPEND WTF_SOURCES
    cutefish/ChassisTypeCutefish.cpp
    cutefish/FileSystemCutefish.cpp

    generic/MainThreadGeneric.cpp
    generic/RunLoopGeneric.cpp
    generic/WorkQueueGeneric.cpp

    linux/CurrentProcessMemoryStatus.cpp
    linux/HighPriorityThreads.cpp
    linux/MemoryFootprintLinux.cpp

    posix/CPUTimePOSIX.cpp
    posix/FileHandlePOSIX.cpp
    posix/FileSystemPOSIX.cpp
    posix/MappedFileDataPOSIX.cpp
    posix/OSAllocatorPOSIX.cpp
    posix/ThreadingPOSIX.cpp

    text/unix/TextBreakIteratorInternalICUUnix.cpp

    unix/LanguageUnix.cpp
    unix/LoggingUnix.cpp
    unix/MemoryPressureHandlerUnix.cpp
    unix/UniStdExtrasUnix.cpp
)

list(APPEND WTF_PUBLIC_HEADERS
    PlatformEnableCutefish.h

    glib/ActivityObserver.h
    glib/ChassisType.h

    linux/CurrentProcessMemoryStatus.h
    linux/HighPriorityThreads.h
    linux/ProcessMemoryFootprint.h

    posix/SocketPOSIX.h

    unix/UnixFileDescriptor.h
)

list(APPEND WTF_LIBRARIES
    Threads::Threads
)
