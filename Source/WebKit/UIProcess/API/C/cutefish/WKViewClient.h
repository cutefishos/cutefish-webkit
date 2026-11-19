// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKViewClient_h
#define WKViewClient_h

#include <WebKit/WKBase.h>
#include <WebKit/WKGeometry.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    kWKViewBufferTypeDMABuf,
    kWKViewBufferTypeSharedMemory
};
typedef uint8_t WKViewBufferType;

// A buffer the web process renders into. The file descriptors of a DMA-BUF
// belong to the client from bufferCreated on; shared memory stays readable
// at data until bufferDestroyed. fourcc is a DRM format code.
typedef struct WKViewBuffer {
    uint64_t identifier;
    WKViewBufferType type;
    uint32_t width;
    uint32_t height;
    uint32_t fourcc;
    uint64_t modifier;
    uint32_t planeCount;
    int fds[4];
    uint32_t offsets[4];
    uint32_t strides[4];
    const void* data;
    size_t size;
} WKViewBuffer;

typedef void (*WKViewBufferCreatedCallback)(WKViewRef, const WKViewBuffer*, const void* clientInfo);
typedef void (*WKViewBufferDestroyedCallback)(WKViewRef, uint64_t bufferIdentifier, const void* clientInfo);

// The buffer holds a new frame once renderingFence, when not -1, signals; the
// fence belongs to the client. Call WKViewFrameDisplayed() once the frame is
// on screen and WKViewReleaseBuffer() once the buffer is no longer read.
typedef void (*WKViewFrameCallback)(WKViewRef, uint64_t bufferIdentifier, const WKRect* damage, size_t damageCount, int renderingFence, const void* clientInfo);

// name is a CSS cursor keyword, such as "default", "pointer" or "ew-resize".
typedef void (*WKViewSetCursorCallback)(WKViewRef, const char* name, const void* clientInfo);
typedef void (*WKViewToolTipChangedCallback)(WKViewRef, const char* toolTip, const void* clientInfo);

// Whether the focused element takes text from an input method, and its
// caret in view coordinates.
typedef void (*WKViewInputMethodStateCallback)(WKViewRef, bool enabled, WKRect caret, const void* clientInfo);

enum {
    kWKViewMenuItemAction,
    kWKViewMenuItemCheckable,
    kWKViewMenuItemSeparator,
    kWKViewMenuItemSubmenu
};
typedef uint32_t WKViewMenuItemType;

// A menu item; the items of a submenu follow it one level deeper. tag is a
// WKContextMenuItemTag in a context menu and 0 in a popup.
typedef struct WKViewMenuItem {
    const char* title;
    WKViewMenuItemType type;
    uint32_t tag;
    uint32_t depth;
    bool enabled;
    bool checked;
    bool isLabel;
} WKViewMenuItem;

// The popup of a select element over its rectangle in view coordinates.
// Answer with WKViewPopupMenuItemSelected once it closes.
typedef void (*WKViewShowPopupMenuCallback)(WKViewRef, const WKViewMenuItem* items, size_t count, int32_t selectedIndex, WKRect rect, const void* clientInfo);
typedef void (*WKViewHidePopupMenuCallback)(WKViewRef, const void* clientInfo);

// A context menu at a point in view coordinates. Answer with
// WKViewContextMenuItemSelected once it closes.
typedef void (*WKViewShowContextMenuCallback)(WKViewRef, const WKViewMenuItem* items, size_t count, WKPoint location, const void* clientInfo);

// A wheel event that nothing in the page scrolled for, as at the end of
// the content; delta as the client sent it.
typedef void (*WKViewWheelEventNotHandledCallback)(WKViewRef, WKSize delta, const void* clientInfo);
// The colour shown past the ends of the page, as when it is stretched;
// sRGB components from 0 to 1, alpha 0 when the page has none.
typedef void (*WKViewUnderPageBackgroundColorChangedCallback)(WKViewRef, double red, double green, double blue, double alpha, const void* clientInfo);
// The size of the main frame's contents.
typedef void (*WKViewContentsSizeChangedCallback)(WKViewRef, WKSize size, const void* clientInfo);

typedef void (*WKViewEnterFullScreenCallback)(WKViewRef, const void* clientInfo);
typedef void (*WKViewExitFullScreenCallback)(WKViewRef, const void* clientInfo);

// What a drag carries; any field may be missing (null, or a zero count).
// fileURLs are file: URLs; image holds an image's encoded bytes.
typedef struct WKViewDragData {
    const char* text;
    const char* markup;
    const char* url;
    const char* urlTitle;
    const char* const* fileURLs;
    size_t fileURLCount;
    const void* image;
    size_t imageSize;
} WKViewDragData;

// Drag operations, combined as bits.
enum {
    kWKViewDragOperationCopy = 1 << 0,
    kWKViewDragOperationLink = 1 << 1,
    kWKViewDragOperationGeneric = 1 << 2,
    kWKViewDragOperationPrivate = 1 << 3,
    kWKViewDragOperationMove = 1 << 4,
    kWKViewDragOperationDelete = 1 << 5
};

// The page starts dragging its data with the operations it allows. The
// image, when there is one, is premultiplied BGRA rows, top first, with the
// pointer at hotspot in it. Answer with WKViewDragSourceEnded once the drag
// ends; without a callback the drag is cancelled.
typedef void (*WKViewStartDragCallback)(WKViewRef, const WKViewDragData*, uint32_t operations, const void* imagePixels, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride, WKPoint hotspot, const void* clientInfo);

typedef struct WKViewClientBase {
    int version;
    const void* clientInfo;
} WKViewClientBase;

typedef struct WKViewClientV0 {
    WKViewClientBase base;

    // version 0
    WKViewBufferCreatedCallback bufferCreated;
    WKViewBufferDestroyedCallback bufferDestroyed;
    WKViewFrameCallback frame;
    WKViewSetCursorCallback setCursor;
    WKViewToolTipChangedCallback toolTipChanged;
    WKViewEnterFullScreenCallback enterFullScreen;
    WKViewExitFullScreenCallback exitFullScreen;
    WKViewInputMethodStateCallback inputMethodStateChanged;
    WKViewShowPopupMenuCallback showPopupMenu;
    WKViewHidePopupMenuCallback hidePopupMenu;
    WKViewShowContextMenuCallback showContextMenu;
    WKViewWheelEventNotHandledCallback wheelEventNotHandled;
    WKViewContentsSizeChangedCallback contentsSizeChanged;
    WKViewUnderPageBackgroundColorChangedCallback underPageBackgroundColorChanged;
} WKViewClientV0;

typedef struct WKViewClientV1 {
    WKViewClientBase base;

    // version 0
    WKViewBufferCreatedCallback bufferCreated;
    WKViewBufferDestroyedCallback bufferDestroyed;
    WKViewFrameCallback frame;
    WKViewSetCursorCallback setCursor;
    WKViewToolTipChangedCallback toolTipChanged;
    WKViewEnterFullScreenCallback enterFullScreen;
    WKViewExitFullScreenCallback exitFullScreen;
    WKViewInputMethodStateCallback inputMethodStateChanged;
    WKViewShowPopupMenuCallback showPopupMenu;
    WKViewHidePopupMenuCallback hidePopupMenu;
    WKViewShowContextMenuCallback showContextMenu;
    WKViewWheelEventNotHandledCallback wheelEventNotHandled;
    WKViewContentsSizeChangedCallback contentsSizeChanged;
    WKViewUnderPageBackgroundColorChangedCallback underPageBackgroundColorChanged;

    // version 1
    WKViewStartDragCallback startDrag;
} WKViewClientV1;

#ifdef __cplusplus
}
#endif

#endif /* WKViewClient_h */
