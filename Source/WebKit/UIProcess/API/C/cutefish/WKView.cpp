// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause

#include "config.h"
#include "WKView.h"

#include "APIPageConfiguration.h"
#include "CutefishWebView.h"
#include "WKAPICast.h"
#include "WebPageProxy.h"

using namespace WebKit;

WKViewRef WKViewCreate(WKPageConfigurationRef configuration, WKSize size)
{
    return toAPI(&CutefishWebView::create(*toImpl(configuration), toIntSize(size)).leakRef());
}

void WKViewSetViewClient(WKViewRef view, const WKViewClientBase* client)
{
    toImpl(view)->setClient(client);
}

WKPageRef WKViewGetPage(WKViewRef view)
{
    return toAPI(&toImpl(view)->page());
}

void WKViewSetSize(WKViewRef view, WKSize size)
{
    toImpl(view)->setSize(toIntSize(size));
}

void WKViewSetDeviceScaleFactor(WKViewRef view, double scaleFactor)
{
    toImpl(view)->setDeviceScaleFactor(scaleFactor);
}

void WKViewSetObscuredContentInsets(WKViewRef view, float top, float right, float bottom, float left)
{
    toImpl(view)->page().setObscuredContentInsets(WebCore::FloatBoxExtent(top, right, bottom, left));
}

void WKViewSetDarkAppearance(WKViewRef view, bool dark)
{
    toImpl(view)->setDarkAppearance(dark);
}

void WKViewSetFocused(WKViewRef view, bool focused)
{
    toImpl(view)->setActivityState(WebCore::ActivityState::IsFocused, focused);
}

void WKViewSetActive(WKViewRef view, bool active)
{
    toImpl(view)->setActivityState(WebCore::ActivityState::WindowIsActive, active);
}

void WKViewSetVisible(WKViewRef view, bool visible)
{
    toImpl(view)->setActivityState(WebCore::ActivityState::IsVisible, visible);
}

void WKViewSetInWindow(WKViewRef view, bool inWindow)
{
    toImpl(view)->setActivityState(WebCore::ActivityState::IsInWindow, inWindow);
}

void WKViewSendMouseEvent(WKViewRef view, const WKMouseEvent* event)
{
    toImpl(view)->sendMouseEvent(*event);
}

void WKViewSendWheelEvent(WKViewRef view, const WKWheelEvent* event)
{
    toImpl(view)->sendWheelEvent(*event);
}

void WKViewSendKeyboardEvent(WKViewRef view, const WKKeyboardEvent* event)
{
    toImpl(view)->sendKeyboardEvent(*event);
}

void WKViewSendTouchEvent(WKViewRef view, const WKTouchEvent* event)
{
    toImpl(view)->sendTouchEvent(*event);
}

void WKViewFrameDisplayed(WKViewRef view)
{
    toImpl(view)->frameDisplayed();
}

void WKViewReleaseBuffer(WKViewRef view, uint64_t bufferIdentifier, int releaseFence)
{
    toImpl(view)->releaseBuffer(bufferIdentifier, UnixFileDescriptor { releaseFence, UnixFileDescriptor::Adopt });
}

void WKViewSetComposition(WKViewRef view, const char* text, uint32_t cursor)
{
    toImpl(view)->setComposition(String::fromUTF8(text), cursor);
}

void WKViewConfirmComposition(WKViewRef view, const char* text)
{
    toImpl(view)->confirmComposition(String::fromUTF8(text));
}

void WKViewCancelComposition(WKViewRef view)
{
    toImpl(view)->cancelComposition();
}

void WKViewPopupMenuItemSelected(WKViewRef view, int32_t index)
{
    toImpl(view)->popupMenuItemSelected(index);
}

void WKViewContextMenuItemSelected(WKViewRef view, int32_t index)
{
    toImpl(view)->contextMenuItemSelected(index);
}

void WKViewDragEntered(WKViewRef view, const WKViewDragData* data, WKPoint point, uint32_t operations)
{
    toImpl(view)->dragEntered(*data, toIntPoint(point), operations);
}

void WKViewDragUpdated(WKViewRef view, const WKViewDragData* data, WKPoint point, uint32_t operations)
{
    toImpl(view)->dragUpdated(*data, toIntPoint(point), operations);
}

void WKViewDragExited(WKViewRef view, const WKViewDragData* data, WKPoint point, uint32_t operations)
{
    toImpl(view)->dragExited(*data, toIntPoint(point), operations);
}

void WKViewPerformDrag(WKViewRef view, const WKViewDragData* data, WKPoint point, uint32_t operations)
{
    toImpl(view)->performDrag(*data, toIntPoint(point), operations);
}

uint32_t WKViewGetDragOperation(WKViewRef view)
{
    return toImpl(view)->dragOperation();
}

void WKViewDragSourceEnded(WKViewRef view, WKPoint point, uint32_t operation)
{
    toImpl(view)->dragSourceEnded(toIntPoint(point), operation);
}

void WKViewRequestExitFullScreen(WKViewRef view)
{
    toImpl(view)->requestExitFullScreen();
}

bool WKViewIsFullScreen(WKViewRef view)
{
    return toImpl(view)->isFullScreen();
}
