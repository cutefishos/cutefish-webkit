include(GNUInstallDirs)
include(Headers.cmake)
include(Platform/Curl.cmake)
include(Platform/Skia.cmake)
include(ModernMediaControlsGResources.cmake)

if (ENABLE_PDFJS)
    include(PdfJSGResources.cmake)
endif ()

# The network cache keeps its data in GLib buffers instead of the curl backend's.
list(REMOVE_ITEM WebKit_SOURCES
    NetworkProcess/cache/NetworkCacheDataCurl.cpp
    NetworkProcess/cache/NetworkCacheIOChannelCurl.cpp
)

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

    UIProcess/API/C/cutefish/WKContextSandbox.cpp
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
)

list(APPEND WebKit_DERIVED_SOURCES
    ${WebKit_DERIVED_SOURCES_DIR}/ModernMediaControlsGResourceBundle.c
    ${WebKit_DERIVED_SOURCES_DIR}/WebKitResourcesGResourceBundle.c
)

WEBKIT_BUILD_MODERN_MEDIA_CONTROLS_GRESOURCES(${WebKit_DERIVED_SOURCES_DIR})

if (ENABLE_PDFJS)
    list(APPEND WebKit_DERIVED_SOURCES
        ${WebKit_DERIVED_SOURCES_DIR}/PdfJSGResourceBundle.c
        ${WebKit_DERIVED_SOURCES_DIR}/PdfJSGResourceBundleExtras.c
    )

    WEBKIT_BUILD_PDFJS_GRESOURCES(${WebKit_DERIVED_SOURCES_DIR})
endif ()

set(WebKitResources "")
list(APPEND WebKitResources
  "        <file alias='images/missingImage@2x'>missingImage@2x.png</file>\n"
  "        <file alias='images/missingImage@3x'>missingImage@3x.png</file>\n"
  "        <file alias='images/missingImage'>missingImage.png</file>\n"
  "        <file alias='images/panIcon'>panIcon.png</file>\n"
  "        <file alias='images/textAreaResizeCorner@2x'>textAreaResizeCorner@2x.png</file>\n"
  "        <file alias='images/textAreaResizeCorner'>textAreaResizeCorner.png</file>\n"
)

if (ENABLE_WEB_AUDIO)
    list(APPEND WebKitResources
        "        <file alias=\"audio/Composite\">Composite.wav</file>\n"
    )
endif ()

# WebCore looks its images and audio up under WPE's resource path.
file(WRITE ${WebKit_DERIVED_SOURCES_DIR}/WebKitResourcesGResourceBundle.xml
    "<?xml version=1.0 encoding=UTF-8?>\n"
    "<gresources>\n"
    "    <gresource prefix=\"/org/webkitwpe/resources\">\n"
    ${WebKitResources}
    "    </gresource>\n"
    "</gresources>\n"
)

GLIB_COMPILE_RESOURCES(
    OUTPUT        ${WebKit_DERIVED_SOURCES_DIR}/WebKitResourcesGResourceBundle.c
    SOURCE_XML    ${WebKit_DERIVED_SOURCES_DIR}/WebKitResourcesGResourceBundle.xml
    RESOURCE_DIRS ${CMAKE_SOURCE_DIR}/Source/WebCore/Resources
                  ${CMAKE_SOURCE_DIR}/Source/WebCore/platform/audio/resources
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
    "${WEBKIT_DIR}/UIProcess/Launcher/glib"
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

    UIProcess/API/C/cutefish/WKContentWorld.h
    UIProcess/API/C/cutefish/WKContextSandbox.h
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
    GLib::GioUnix
    GLib::Module
    XkbCommon::XkbCommon
)

if (USE_GBM)
    list(APPEND WebKit_LIBRARIES GBM::GBM)
endif ()

if (ENABLE_BUBBLEWRAP_SANDBOX)
    list(APPEND WebKit_LIBRARIES Libseccomp::Libseccomp)
endif ()

if (USE_GSTREAMER)
    list(APPEND WebKit_SYSTEM_INCLUDE_DIRECTORIES
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_AUDIO_INCLUDE_DIRS}
        ${GSTREAMER_PBUTILS_INCLUDE_DIRS}
        ${GSTREAMER_VIDEO_INCLUDE_DIRS}
    )
    list(APPEND WebKit_LIBRARIES
        ${GSTREAMER_LIBRARIES}
    )
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
