// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#ifndef WKView_h
#define WKView_h

#include <WebKit/WKBase.h>
#include <WebKit/WKEventCutefish.h>
#include <WebKit/WKGeometry.h>
#include <WebKit/WKViewClient.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// size is the initial size in logical pixels.
WK_EXPORT WKViewRef WKViewCreate(WKPageConfigurationRef, WKSize size);

WK_EXPORT void WKViewSetViewClient(WKViewRef, const WKViewClientBase*);

WK_EXPORT WKPageRef WKViewGetPage(WKViewRef);

// size is in logical pixels; buffers are size times the scale factor.
WK_EXPORT void WKViewSetSize(WKViewRef, WKSize);
WK_EXPORT void WKViewSetDeviceScaleFactor(WKViewRef, double);
// The edges, in logical pixels, that something drawn over the view covers;
// the page scrolls under them and keeps its content clear of them at rest.
WK_EXPORT void WKViewSetObscuredContentInsets(WKViewRef, float top, float right, float bottom, float left);
// Whether pages see a dark appearance (prefers-color-scheme, form controls).
WK_EXPORT void WKViewSetDarkAppearance(WKViewRef, bool);

WK_EXPORT void WKViewSetFocused(WKViewRef, bool);
WK_EXPORT void WKViewSetActive(WKViewRef, bool);
WK_EXPORT void WKViewSetVisible(WKViewRef, bool);
WK_EXPORT void WKViewSetInWindow(WKViewRef, bool);

WK_EXPORT void WKViewSendMouseEvent(WKViewRef, const WKMouseEvent*);
WK_EXPORT void WKViewSendWheelEvent(WKViewRef, const WKWheelEvent*);
WK_EXPORT void WKViewSendKeyboardEvent(WKViewRef, const WKKeyboardEvent*);
WK_EXPORT void WKViewSendTouchEvent(WKViewRef, const WKTouchEvent*);

// Input method text: a composition with the caret at a UTF-16 offset in it,
// a commit that replaces it, or the composition dropped.
WK_EXPORT void WKViewSetComposition(WKViewRef, const char* text, uint32_t cursor);
WK_EXPORT void WKViewConfirmComposition(WKViewRef, const char* text);
WK_EXPORT void WKViewCancelComposition(WKViewRef);

// The index of the item chosen in the menu shown, or -1 when it closed
// without a choice.
WK_EXPORT void WKViewPopupMenuItemSelected(WKViewRef, int32_t index);
WK_EXPORT void WKViewContextMenuItemSelected(WKViewRef, int32_t index);

WK_EXPORT void WKViewFrameDisplayed(WKViewRef);
// releaseFence, when not -1, signals once the client stops reading the
// buffer; it passes to the view.
WK_EXPORT void WKViewReleaseBuffer(WKViewRef, uint64_t bufferIdentifier, int releaseFence);

// A drag from anywhere over the view, at a point in view coordinates, with
// the operations its source allows. WKViewGetDragOperation answers the
// operation the page last chose, 0 for none; WKViewPerformDrag drops.
WK_EXPORT void WKViewDragEntered(WKViewRef, const WKViewDragData*, WKPoint, uint32_t operations);
WK_EXPORT void WKViewDragUpdated(WKViewRef, const WKViewDragData*, WKPoint, uint32_t operations);
WK_EXPORT void WKViewDragExited(WKViewRef, const WKViewDragData*, WKPoint, uint32_t operations);
WK_EXPORT void WKViewPerformDrag(WKViewRef, const WKViewDragData*, WKPoint, uint32_t operations);
WK_EXPORT uint32_t WKViewGetDragOperation(WKViewRef);
// The drag the page started ended at a point, with the operation done, 0
// when it was cancelled.
WK_EXPORT void WKViewDragSourceEnded(WKViewRef, WKPoint, uint32_t operation);

WK_EXPORT void WKViewRequestExitFullScreen(WKViewRef);
WK_EXPORT bool WKViewIsFullScreen(WKViewRef);

#ifdef __cplusplus
}
#endif

#endif /* WKView_h */
