include(GNUInstallDirs)
include(Headers.cmake)
include(Platform/Curl.cmake)
include(Platform/Skia.cmake)

set(WebKit_OUTPUT_NAME cutefish-webkit)
set(WebProcess_OUTPUT_NAME CutefishWebProcess)
set(NetworkProcess_OUTPUT_NAME CutefishNetworkProcess)

add_definitions(-DLIBDIR="${LIB_INSTALL_DIR}")
add_definitions(-DPKGLIBEXECDIR="${LIBEXEC_INSTALL_DIR}")
add_definitions(-DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}")

list(APPEND WebProcess_SOURCES
    WebProcess/EntryPoint/unix/WebProcessMain.cpp
)
list(APPEND WebProcess_INCLUDE_DIRECTORIES ${WebKit_PRIVATE_INCLUDE_DIRECTORIES})

list(APPEND NetworkProcess_SOURCES
    NetworkProcess/EntryPoint/unix/NetworkProcessMain.cpp
)
list(APPEND NetworkProcess_INCLUDE_DIRECTORIES ${WebKit_PRIVATE_INCLUDE_DIRECTORIES})

list(APPEND WebKit_UNIFIED_SOURCE_LIST_FILES
    "SourcesCutefish.txt"
)

list(APPEND WebKit_SOURCES
    Shared/cutefish/NativeWebEventsCutefish.cpp

    UIProcess/API/C/cutefish/WKDisplaySync.cpp
    UIProcess/API/C/cutefish/WKDownloadDelegate.cpp
    UIProcess/API/C/cutefish/WKMainRunLoop.cpp
    UIProcess/API/C/cutefish/WKURLSchemeHandler.cpp
    UIProcess/API/C/cutefish/WKView.cpp

    UIProcess/API/C/playstation/WKRunLoop.cpp

    UIProcess/cutefish/AcceleratedBackingStore.cpp
    UIProcess/cutefish/CutefishWebView.cpp
    UIProcess/cutefish/DisplayVBlankMonitorCutefish.cpp
    UIProcess/cutefish/WebContextMenuProxyCutefish.cpp
    UIProcess/cutefish/WebPopupMenuProxyCutefish.cpp
    UIProcess/cutefish/PageClientImpl.cpp

    UIProcess/WebsiteData/cutefish/WebsiteDataStoreCutefish.cpp

    WebProcess/InjectedBundle/cutefish/InjectedBundleCutefish.cpp
)

list(APPEND WebKit_SERIALIZATION_IN_FILES
    Shared/glib/AvailableInputDevices.serialization.in
    Shared/glib/DMABufBufferAttributes.serialization.in
    Shared/glib/InputMethodState.serialization.in
    Shared/glib/RenderProcessInfo.serialization.in
    Shared/glib/RendererBufferFormat.serialization.in
    Shared/glib/RendererBufferTransportMode.serialization.in
    Shared/glib/SelectionData.serialization.in
    Shared/glib/SystemSettings.serialization.in
)

if (USE_GBM)
    list(APPEND WebKit_SERIALIZATION_IN_FILES
        Shared/gbm/DRMDevice.serialization.in
    )
endif ()

list(APPEND WebKit_MESSAGES_IN_FILES
    UIProcess/glib/AcceleratedBackingStore

    WebProcess/WebPage/CoordinatedGraphics/AcceleratedSurface

    WebProcess/glib/SystemSettingsManager
)

list(APPEND WebKit_PRIVATE_INCLUDE_DIRECTORIES
    "${WEBKIT_DIR}/Platform/IPC/unix"
    "${WEBKIT_DIR}/Platform/classifier"
    "${WEBKIT_DIR}/Platform/generic"
    "${WEBKIT_DIR}/Shared/API/c/cutefish"
    "${WEBKIT_DIR}/Shared/cutefish"
    "${WEBKIT_DIR}/Shared/glib"
    "${WEBKIT_DIR}/UIProcess/API/C/cutefish"
    "${WEBKIT_DIR}/UIProcess/API/C/glib"
    "${WEBKIT_DIR}/UIProcess/API/C/playstation"
    "${WEBKIT_DIR}/UIProcess/CoordinatedGraphics"
    "${WEBKIT_DIR}/UIProcess/Launcher/cutefish"
    "${WEBKIT_DIR}/UIProcess/cutefish"
    "${WEBKIT_DIR}/UIProcess/glib"
    "${WEBKIT_DIR}/UIProcess/linux"
    "${WEBKIT_DIR}/WebProcess/WebPage/CoordinatedGraphics"
    "${WEBKIT_DIR}/WebProcess/WebPage/glib"
    "${WEBKIT_DIR}/WebProcess/glib"
)

list(APPEND WebKit_PUBLIC_FRAMEWORK_HEADERS
    Shared/API/c/cutefish/WKBaseCutefish.h
    Shared/API/c/cutefish/WKEventCutefish.h

    UIProcess/API/C/cutefish/WKDisplaySync.h
    UIProcess/API/C/cutefish/WKDownloadDelegate.h
    UIProcess/API/C/cutefish/WKMainRunLoop.h
    UIProcess/API/C/cutefish/WKPasteboard.h
    UIProcess/API/C/cutefish/WKURLSchemeHandler.h
    UIProcess/API/C/cutefish/WKView.h
    UIProcess/API/C/cutefish/WKViewClient.h

    UIProcess/API/C/playstation/WKRunLoop.h
)

list(APPEND WebKit_LIBRARIES
    XkbCommon::XkbCommon
)

if (USE_GBM)
    list(APPEND WebKit_LIBRARIES GBM::GBM)
endif ()

configure_file(cutefish-webkit.pc.in ${CMAKE_BINARY_DIR}/cutefish-webkit.pc @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/cutefish-webkit.pc"
    DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)

install(DIRECTORY "${WebKit_FRAMEWORK_HEADERS_DIR}/WebKit"
    DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/cutefish-webkit"
    FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY "${JavaScriptCore_FRAMEWORK_HEADERS_DIR}/JavaScriptCore"
    DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/cutefish-webkit"
    FILES_MATCHING PATTERN "*.h"
)
