list(APPEND WTF_SOURCES
    generic/MainThreadGeneric.cpp
    generic/WorkQueueGeneric.cpp

    glib/Application.cpp
    glib/ChassisType.cpp
    glib/FilePathWatcher.cpp
    glib/FileSystemGlib.cpp
    glib/GMallocString.cpp
    glib/GRefPtr.cpp
    glib/GSocketMonitor.cpp
    glib/GSpanExtras.cpp
    glib/RunLoopGLib.cpp
    glib/Sandbox.cpp
    glib/SocketConnection.cpp
    glib/TimeZoneGLib.cpp

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
    glib/Application.h
    glib/ChassisType.h
    glib/FilePathWatcher.h
    glib/GMallocString.h
    glib/GRefPtr.h
    glib/GSocketMonitor.h
    glib/GSpanExtras.h
    glib/GThreadSafeWeakPtr.h
    glib/GTypedefs.h
    glib/GUniquePtr.h
    glib/GWeakPtr.h
    glib/RunLoopSourcePriority.h
    glib/Sandbox.h
    glib/SocketConnection.h
    glib/SysprofAnnotator.h
    glib/WTFGType.h

    linux/CurrentProcessMemoryStatus.h
    linux/HighPriorityThreads.h
    linux/ProcessMemoryFootprint.h

    posix/SocketPOSIX.h

    unix/UnixFileDescriptor.h
)

list(APPEND WTF_LIBRARIES
    GLib::Gio
    Threads::Threads
)
