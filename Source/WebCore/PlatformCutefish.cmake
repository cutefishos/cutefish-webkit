include(platform/Adwaita.cmake)
include(platform/Curl.cmake)
include(platform/GStreamer.cmake)
include(platform/ImageDecoders.cmake)
include(platform/OpenSSL.cmake)
include(platform/Skia.cmake)
include(platform/TextureMapper.cmake)

list(APPEND WebCore_UNIFIED_SOURCE_LIST_FILES
    "SourcesCutefish.txt"
)

list(APPEND WebCore_PRIVATE_INCLUDE_DIRECTORIES
    "${WEBCORE_DIR}/crypto/openssl"
    "${WEBCORE_DIR}/platform/generic"
    "${WEBCORE_DIR}/platform/glib"
    "${WEBCORE_DIR}/platform/graphics/egl"
    "${WEBCORE_DIR}/platform/graphics/epoxy"
    "${WEBCORE_DIR}/platform/graphics/gbm"
    "${WEBCORE_DIR}/platform/graphics/opengl"
    "${WEBCORE_DIR}/platform/graphics/opentype"
    "${WEBCORE_DIR}/platform/mediacapabilities"
    "${WEBCORE_DIR}/platform/mediastream/gstreamer"
    "${WEBCORE_DIR}/platform/text/icu"
    "${WEBCORE_DIR}/platform/video-codecs"
    "${WEBCORE_DIR}/platform/wpe"
)

list(APPEND WebCore_PRIVATE_FRAMEWORK_HEADERS
    platform/glib/SelectionData.h
    platform/glib/SystemSettings.h
    platform/graphics/DMABufBuffer.h
    platform/graphics/DMABufBufferAttributes.h
    platform/graphics/egl/PlatformDisplayDefault.h
    platform/graphics/egl/PlatformDisplaySurfaceless.h
    platform/graphics/gbm/GBMVersioning.h
    platform/graphics/gbm/PlatformDisplayGBM.h
)

set(WebCore_USER_AGENT_SCRIPTS_DEPENDENCIES ${WEBCORE_DIR}/rendering/adwaita/RenderThemeAdwaita.cpp)

list(APPEND WebCore_LIBRARIES
    XkbCommon::XkbCommon
)

if (USE_GBM)
    list(APPEND WebCore_LIBRARIES GBM::GBM)
elseif (USE_LIBDRM)
    list(APPEND WebCore_LIBRARIES LibDRM::LibDRM)
endif ()

# This sets the maximum amount of memory that BitmapTexturePool can hold before being more
# aggressive trying to release the unused textures.
list(APPEND WebCore_PRIVATE_DEFINITIONS
     BITMAP_TEXTURE_POOL_MAX_SIZE_IN_MB=80
)
